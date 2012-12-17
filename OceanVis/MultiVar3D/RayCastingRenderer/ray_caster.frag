/*****************************************************************************/
/**
 *  @file   ray_caster.frag
 *  @author Naohisa Sakamoto
 */
/*----------------------------------------------------------------------------
 *
 *  Copyright 2007 Visualization Laboratory, Kyoto University.
 *  All rights reserved.
 *  See http://www.viz.media.kyoto-u.ac.jp/kvs/copyright/ for details.
 *
 *  $Id: ray_caster.frag 1041 2012-03-14 15:28:49Z naohisa.sakamoto@gmail.com $
 */
/*****************************************************************************/

// shading.h
struct Shading
{
    float Ka; // ambient
    float Kd; // diffuse
    float Ks; // specular
    float S;  // shininess
};

vec3 ShadingNone( in Shading shading, in vec3 color )
{
    return( color );
}

vec3 ShadingLambert( in Shading shading, in vec3 color, in vec3 L, in vec3 N )
{
    #if defined( ENABLE_TWO_SIDE_LIGHTING )
    float dd = abs( dot( N, L ) );
    #else
    float dd = max( dot( N, L ), 0.0 );
    #endif
    
    float Ia = shading.Ka;
    float Id = shading.Kd * dd;
    
    return( color * ( Ia + Id ) );
}

vec3 ShadingPhong( in Shading shading, in vec3 color, in vec3 L, in vec3 N, in vec3 V )
{
    vec3 R = reflect( -L, N );
    #if defined( ENABLE_TWO_SIDE_LIGHTING )
    float dd = abs( dot( N, L ) );
    float ds = pow( abs( dot( R, V ) ), shading.S );
    #else
    float dd = max( dot( N, L ), 0.0 );
    float ds = pow( max( dot( R, V ), 0.0 ), shading.S );
    #endif
    if ( dd <= 0.0 ) ds = 0.0;
    
    float Ia = shading.Ka;
    float Id = shading.Kd * dd;
    float Is = shading.Ks * ds;
    
    return( color * ( Ia + Id ) + Is );
}

vec3 ShadingBlinnPhong( in Shading shading, in vec3 color, in vec3 L, in vec3 N, in vec3 V )
{
    vec3 H = normalize( L + V );
    #if defined( ENABLE_TWO_SIDE_LIGHTING )
    float dd = abs( dot( N, L ) );
    float ds = pow( abs( dot( H, N ) ), shading.S );
    #else
    float dd = max( dot( N, L ), 0.0 );
    float ds = pow( max( dot( H, N ), 0.0 ), shading.S );
    #endif
    if ( dd <= 0.0 ) ds = 0.0;
    
    float Ia = shading.Ka;
    float Id = shading.Kd * dd;
    float Is = shading.Ks * ds;
    
    return( color * ( Ia + Id ) + Is );
}

// volume.h
struct Volume
{
    vec3 resolution; // volume resolution
    vec3 resolution_ratio; // ratio of the resolution (256x256x128 => 1:1:0.5)
    vec3 resolution_reciprocal; // reciprocal number of the resolution
    float min_range; // min. range of the value
    float max_range; // max. range of the value
    sampler3D data; // volume data (scalar field)
};

// transfer_function
struct TransferFunction
{
    sampler1D data; // transfer function data
    float min_value; // min. scalar value
    float max_value; // max. scalar value
};

// ray_caster.frag
uniform sampler2D        entry_points;      // entry points (front face)
uniform sampler2D        exit_points;       // exit points (back face)
uniform vec3             offset;            // offset width for the gradient
uniform float            dt;                // sampling step
uniform float            opaque;            // opaque value
uniform vec3             light_position;    // light position in the object coordinate
uniform vec3             camera_position;   // camera position in the object coordinate
uniform Volume           volume;            // volume data
uniform Shading          shading;           // shading parameter
uniform TransferFunction transfer_function; // 1D transfer function
uniform sampler2D        jittering_texture; // texture for jittering
uniform float            width;             // screen width
uniform float            height;            // screen height
uniform sampler2D        depth_texture;     // depth texture for depth buffer
uniform sampler2D        color_texture;     // color texture for color buffer
uniform float            to_zw1;            // scaling parameter: (f*n)/(f-n)
uniform float            to_zw2;            // scaling parameter: 0.5*((f+n)/(f-n))+0.5
uniform float            to_ze1;            // scaling parameter: 0.5 + 0.5*((f+n)/(f-n))
uniform float            to_ze2;            // scaling parameter: (f-n)/(f*n)

uniform sampler3D        volume2;           // for the second volume;
uniform sampler3D        volume3;           // for the third volume;
uniform sampler3D        transfer_function3d;// 2d tansfer function
uniform float            tfunc_width;
uniform float            tfunc_height;
uniform float            tfunc_depth;

vec3 estimateGradient( in sampler3D v, in vec3 p, in vec3 o )
{
    float s0 = texture3D( v, p + vec3( o.x, 0.0, 0.0 ) ).w;
    float s1 = texture3D( v, p + vec3( 0.0, o.y, 0.0 ) ).w;
    float s2 = texture3D( v, p + vec3( 0.0, 0.0, o.z ) ).w;
    float s3 = texture3D( v, p - vec3( o.x, 0.0, 0.0 ) ).w;
    float s4 = texture3D( v, p - vec3( 0.0, o.y, 0.0 ) ).w;
    float s5 = texture3D( v, p - vec3( 0.0, 0.0, o.z ) ).w;

    return( vec3( s3 - s0, s4 - s1, s5 - s2 ) );
}

vec3 estimateGradient8( in sampler3D v, in vec3 p, in vec3 o )
{
    vec3 g0 = estimateGradient( v, p, o );
    vec3 g1 = estimateGradient( v, p + vec3( -o.x, -o.y, -o.z ), o );
    vec3 g2 = estimateGradient( v, p + vec3(  o.x,  o.y,  o.z ), o );
    vec3 g3 = estimateGradient( v, p + vec3( -o.x,  o.y, -o.z ), o );
    vec3 g4 = estimateGradient( v, p + vec3(  o.x, -o.y,  o.z ), o );
    vec3 g5 = estimateGradient( v, p + vec3( -o.x, -o.y,  o.z ), o );
    vec3 g6 = estimateGradient( v, p + vec3(  o.x,  o.y, -o.z ), o );
    vec3 g7 = estimateGradient( v, p + vec3( -o.x,  o.y,  o.z ), o );
    vec3 g8 = estimateGradient( v, p + vec3(  o.x, -o.y, -o.z ), o );
    vec3 mix0 = mix( mix( g1, g2, 0.5 ), mix( g3, g4, 0.5 ), 0.5 );
    vec3 mix1 = mix( mix( g5, g6, 0.5 ), mix( g7, g8, 0.5 ), 0.5 );

    return( mix( g0, mix( mix0, mix1, 0.5 ), 0.75 ) );
}

float ray_depth( in float t, in float entry_depth, in float exit_depth )
{
    float zw_front = entry_depth;
    float ze_front = 1.0 / ((zw_front - to_ze1)*to_ze2);

    float zw_back = exit_depth;
    float ze_back = 1.0 / ((zw_back - to_ze1)*to_ze2);

    float ze_current = ze_front + t * (ze_back - ze_front);
    float zw_current = (1.0/ze_current)*to_zw1 + to_zw2;

    return zw_current;
}

void main( void )
{
    // Entry and exit point.
    vec2 index = vec2( gl_FragCoord.x / width, gl_FragCoord.y / height );
    vec3 entry_point = ( volume.resolution - vec3(1.0) ) * texture2D( entry_points, index ).xyz;
    vec3 exit_point = ( volume.resolution - vec3(1.0) ) * texture2D( exit_points, index ).xyz;
    if ( entry_point == exit_point ) { discard; } // out of cube

    // Ray direction.
    vec3 direction = dt * normalize( exit_point - entry_point );

    // Stochastic jittering.
#if defined( ENABLE_JITTERING )
    entry_point = entry_point + 0.1 * direction * texture2D( jittering_texture, gl_FragCoord.xy / 32.0 ).x;
#endif

    // Ray traversal.
    float segment = distance( exit_point, entry_point );
    int nsteps = int( floor( segment / dt ) );
    vec3 position = entry_point;
    vec4 dst = vec4( 0.0, 0.0, 0.0, 0.0 );

    // Transfer function scale.
//    float tfunc_scale = 1.0 / ( volume.max_value - volume.min_value );
//    float tfunc_scale1 = 1.0 / tfunc_width;
//    float tfunc_scale2 = 1.0 / tfunc_height;

    float entry_depth = texture2D( entry_points, index ).w;
    float exit_depth = texture2D( exit_points, index ).w;
    float depth0 = texture2D( depth_texture, index ).x;
    vec3 color0 = texture2D( color_texture, index ).rgb;
    for ( int i = 0; i < nsteps; i++ )
    {
        // Get the scalar value from the 3D texture.
        vec3 volume_index = vec3( position / ( volume.resolution - vec3(1.0) ) );
        vec4 value1 = texture3D( volume.data, volume_index );
        vec4 value2 = texture3D( volume2, volume_index );
        vec4 value3 = texture3D( volume3, volume_index );
        float scalar1 = mix( volume.min_range, volume.max_range, value1.w );
        float scalar2 = mix( volume.min_range, volume.max_range, value2.w );
        float scalar3 = mix( volume.min_range, volume.max_range, value2.w );

        // Get the source color from the transfer function.
//        float tfunc_index = ( scalar - volume.min_value ) * tfunc_scale;
//        float tfunc_index_width = scalar1 * tfunc_scale1;
//        float tfunc_index_height = scalar2 * tfunc_scale2;
        vec4 src = texture3D( transfer_function3d, vec3( scalar1, scalar2, scalar3 ) );
        if ( src.a != 0.0 )
        {
            // Get the normal vector.
            vec3 offset_index = vec3( volume.resolution_reciprocal );
            vec3 normal = estimateGradient( volume.data, volume_index, offset_index );

            vec3 L = normalize( light_position - position );
            vec3 N = normalize( gl_NormalMatrix * normal );

#if   defined( ENABLE_LAMBERT_SHADING )
            src.rgb = ShadingLambert( shading, src.rgb, L, N );

#elif defined( ENABLE_PHONG_SHADING )
            vec3 V = normalize( camera_position - position );
            src.rgb = ShadingPhong( shading, src.rgb, L, N, V );

#elif defined( ENABLE_BLINN_PHONG_SHADING )
            vec3 V = normalize( camera_position - position );
            src.rgb = ShadingBlinnPhong( shading, src.rgb, L, N, V );

#else // DISABLE SHADING
            src.rgb = ShadingNone( shading, src.rgb );
#endif

            // Front-to-back composition.
            dst.rgb += ( 1.0 - dst.a ) * src.a * src.rgb;
            dst.a += ( 1.0 - dst.a ) * src.a;

            // Early ray termination.
            if ( dst.a > opaque )
            {
                dst.a = 1.0;
                break; // break
            }
        }

        float t = float(i) / float( nsteps - 1 );
        float depth = ray_depth( t, entry_depth, exit_depth );
        if ( depth > depth0 )
        {
            dst.rgb += ( 1.0 - dst.a ) * color0.rgb;
            dst.a = 1.0;
            break;
        }

        position += direction;
    }

    gl_FragColor = dst;
}
