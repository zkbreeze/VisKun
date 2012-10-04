/*****************************************************************************/
/**
 *  @file   polygon.frag
 *  @author Jun Nishimura
 */
/*----------------------------------------------------------------------------
 *
 *  Copyright (c) Visualization Laboratory, Kyoto University.
 *  All rights reserved.
 *  See http://www.viz.media.kyoto-u.ac.jp/kvs/copyright/ for details.
 *
 *  $Id$
 */
/*****************************************************************************/
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

vec3 estimateGradient( in sampler3D v, in vec3 p, in vec3 o )
{
    float s0 = texture3D( v, p + vec3( o.x, 0.0, 0.0 ) ).x;
    float s1 = texture3D( v, p + vec3( 0.0, o.y, 0.0 ) ).x;
    float s2 = texture3D( v, p + vec3( 0.0, 0.0, o.z ) ).x;
    float s3 = texture3D( v, p - vec3( o.x, 0.0, 0.0 ) ).x;
    float s4 = texture3D( v, p - vec3( 0.0, o.y, 0.0 ) ).x;
    float s5 = texture3D( v, p - vec3( 0.0, 0.0, o.z ) ).x;

    return( vec3( s3 - s0, s4 - s1, s5 - s2 ) );
}

varying vec3 position;
//varying vec3 normal;
//varying vec2 id;
//varying vec4 tex;

#if defined( ENABLE_EXACT_DEPTH_TESTING )
varying float depth;
#endif

uniform sampler1D transfer_function;
uniform sampler2D random_texture;
uniform sampler3D volume_texture;

uniform vec2 screen_scale; // not used
uniform vec2 screen_scale_inv; // not used

uniform float random_texture_size_inv;
uniform vec2 random_offset;

uniform Shading shading;

void main()
{
    vec3 tex = gl_TexCoord[0].xyz;

    if ( tex.x < 0 || tex.x > 1 || tex.y < 0 || tex.y > 1 || tex.z < 0 || tex.z > 1 )
    { discard; return; }

    vec4 color = texture1D( transfer_function, texture3D( volume_texture, tex ).r );

    if ( color.a == 0.0 ) { discard; return; }

    if ( color.a < 0.99999 )
    {
        vec2 random_position = ( random_offset + gl_FragCoord.xy ) * random_texture_size_inv;
        float randf = texture2D( random_texture, random_position ).a;
        if ( randf > color.a ) { discard; return; }
    }

    vec3 frag_color = color.rgb;

    // Light position.
    vec3 light_position = gl_LightSource[0].position.xyz;

    // Light vector (L) and Normal vector (N)
    vec3 normal = gl_NormalMatrix * estimateGradient( volume_texture, tex, vec3( 1/64.0, 1/64.0, 1/64.0) );
    vec3 L = normalize( light_position - position );
    vec3 N = normalize( normal );

#if   defined( ENABLE_LAMBERT_SHADING )
    vec3 shaded_color = ShadingLambert( shading, frag_color, L, N );

#elif defined( ENABLE_PHONG_SHADING )
    vec3 V = normalize( -position );
    vec3 shaded_color = ShadingPhong( shading, frag_color, L, N, V );

#elif defined( ENABLE_BLINN_PHONG_SHADING )
    vec3 V = normalize( -position );
    vec3 shaded_color = ShadingBlinnPhong( shading, frag_color, L, N, V );

#else // DISABLE SHADING
    vec3 shaded_color = ShadingNone( shading, frag_color );
#endif
    //vec3 shaded_color = frag_color;
    gl_FragColor = vec4( shaded_color, 1.0 );

#if defined( ENABLE_EXACT_DEPTH_TESTING )
    gl_FragDepth = depth;
#endif
}
