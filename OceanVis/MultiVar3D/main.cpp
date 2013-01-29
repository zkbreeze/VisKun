//
//  main.cpp
//  
//
//  Created by Kun Zhao on 12/12/05.
//  Copyright (c) 2012 Kyoto University. All rights reserved.
//

#include <iostream>
#include <kvs/glut/Application>
#include <kvs/glut/Screen>
#include <kvs/StructuredVolumeObject>
#include <kvs/StructuredVolumeImporter>
#include <kvs/HydrogenVolumeData>
#include "RayCastingRenderer.h"
#include "TransferFunction3D.h"

int main( int argc, char** argv )
{
    kvs::glut::Application app( argc, argv );
    kvs::glut::Screen screen( &app );
    
    kvs::StructuredVolumeObject* volume_s = new kvs::StructuredVolumeImporter( "../Data/20110515fine/s.kvsml" );
    kvs::StructuredVolumeObject* volume_t = new kvs::StructuredVolumeImporter( "../Data/20110515fine/t.kvsml" );
    kvs::StructuredVolumeObject* volume_v = new kvs::StructuredVolumeImporter( "../Data/20110515fine/velocity.kvsml");
        
    // set the 3d transfer function
//    size_t size = 64;
//    size_t width = size;
//    size_t height = size;
//    size_t depth = size;
//    float* tfunc3d = new float[width * height * depth * 4];
//    for ( size_t k = 0; k < depth; k++ )
//        for ( size_t j = 0; j < height; j++ )
//            for ( size_t i = 0; i < width; i++ )
//            {
//                int index = ( i + j * width + k * width * height ) * 4;
//                tfunc3d[index] = (float)i / width;      // red
//                tfunc3d[index + 1] = (float)j / height; // green
//                tfunc3d[index + 2] = 1;                 // blue
//                tfunc3d[index + 3] = (float)k / depth;  // alpha
//            }
    
    kun::TransferFunction3D tfunc3d;
    size_t size = 40;
    size_t width = size;
    size_t height = size;
    size_t depth = size;
    tfunc3d.setResolution( kvs::Vector3f( size ) );
    float* tfunc3d_data = new float[width * height * depth * 4];
    for ( size_t k = 0; k < depth; k++ )
        for ( size_t j = 0; j < height; j++ )
            for ( size_t i = 0; i < width; i++ )
            {
                int index = ( i + j * width + k * width * height ) * 4;
                if ( i <= 10) //water
                {
                    tfunc3d_data[index] = 0;      // red
                    tfunc3d_data[index + 1] = 1; // green
                    tfunc3d_data[index + 2] = 1;                 // blue
                }
                if ( 10 < i && i <= 23 )  //green
                {
                    tfunc3d_data[index] = 0;      // red
                    tfunc3d_data[index + 1] = 1; // green
                    tfunc3d_data[index + 2] = 0;                 // blue
                }
                if ( 23 < i && i <= 26 ) //yellow
                {
                    tfunc3d_data[index] = 1;      // red
                    tfunc3d_data[index + 1] = 1; // green
                    tfunc3d_data[index + 2] = 0;                 // blue
                }
                if ( 26 < i && i < 40 ) // red
                {
                    if( 0 <= j && j < 5 ) //yellow
                    {
                        tfunc3d_data[index] = 1;      // red
                        tfunc3d_data[index + 1] = 1; // green
                        tfunc3d_data[index + 2] = 0;                 // blue
                    }
                    else
                    {
                        tfunc3d_data[index] = 1;      // red
                        tfunc3d_data[index + 1] = 0; // green
                        tfunc3d_data[index + 2] = 0;                 // blue
                    }
                }         
                tfunc3d_data[index + 3] = ((float)k / depth) * 1;  // alpha
            }
//    tfunc3d_data[0] = 198/255.0;
//    tfunc3d_data[1] = 163/255.0;
//    tfunc3d_data[2] = 0;
    tfunc3d_data[0] = 1;
    tfunc3d_data[1] = 1;
    tfunc3d_data[2] = 1;
    tfunc3d_data[3] = 1;
    tfunc3d.setValues( tfunc3d_data );
    tfunc3d.write( "purple.tfunc3d" );
    
    kun::RayCastingRenderer* renderer = new kun::RayCastingRenderer();
    
    renderer->addVolume2( volume_t );
    renderer->addVolume3( volume_v );
    renderer->set3DTransferFunction( tfunc3d );
    renderer->disableShading();
    
    screen.registerObject( volume_s, renderer );
    screen.show();
    
    return( app.run() );   
}