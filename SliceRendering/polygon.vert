/*****************************************************************************/
/**
 *  @file   polygon.vert
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
varying vec3 position;
//varying vec3 normal;
//varying vec2 id;
//varying vec4 tex;

#if defined( ENABLE_EXACT_DEPTH_TESTING )
varying float depth;
#endif

//attribute vec2 identifier;

void main( void )
{
    gl_Position = ftransform();
    //gl_FrontColor = gl_Color;

    position = ( gl_ModelViewMatrix * gl_Vertex ).xyz;
    //normal = ( gl_NormalMatrix * gl_Normal ).xyz;
    //id = identifier;
    gl_TexCoord[0] = gl_MultiTexCoord0;

#if defined( ENABLE_EXACT_DEPTH_TESTING )
    depth = gl_Position.z / gl_Position.w;
#endif
}
