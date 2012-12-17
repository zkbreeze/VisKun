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

int main( int argc, char** argv )
{
    kvs::glut::Application app( argc, argv );
    kvs::glut::Screen screen( &app );
    
    kvs::StructuredVolumeObject* volume_s = new kvs::StructuredVolumeImporter( "../Data/20110515fine/s.kvsml" );
    kvs::StructuredVolumeObject* volume_t = new kvs::StructuredVolumeImporter( "../Data/20110515fine/t.kvsml" );
    kvs::StructuredVolumeObject* volume_v = new kvs::StructuredVolumeImporter( "../Data/20110515fine/velocity.kvsml");
//    kvs::StructuredVolumeObject* volume_s = new kvs::HydrogenVolumeData( kvs::Vector3ui( 128 ));
//    kvs::StructuredVolumeObject* volume_t = new kvs::HydrogenVolumeData( kvs::Vector3ui( 128 ));
//    
//    unsigned char* ori_values = (unsigned char*)volume_s->values().pointer();
//    
//    kvs::AnyValueArray values;
//    float* pvalues = static_cast<float*>( values.allocate<float>( volume_s->nnodes() ) );
//    for( size_t i = 0; i < volume_s->nnodes(); i++ )
//    {
//        *(pvalues++) = *(ori_values++);
//    }
//    
//    volume_s->setValues( values );
//    volume_t->setValues( values );
//    volume_s->updateMinMaxValues();
//    volume_t->updateMinMaxValues();
    
    // set the 2d transfer function
    size_t width = 64;
    size_t height = 64;
    size_t depth = 64;
    float* tfunc3d = new float[width * height * depth * 4];
    for ( size_t k = 0; k < depth; k++ )
        for ( size_t j = 0; j < height; j++ )
            for ( size_t i = 0; i < width; i++ )
            {
                int index = ( i + j * width + k * width * depth ) * 4;
                tfunc3d[index] = (float)i / width; // red
                tfunc3d[index + 1] = (float)j / height; // green
                tfunc3d[index + 2] = 1;              // blue
                tfunc3d[index + 3] = k / depth; //alpha
            }
    
    kvs::glew::kun::RayCastingRenderer* renderer = new kvs::glew::kun::RayCastingRenderer();
    
    renderer->addVolume2( volume_t );
    renderer->addVolume3( volume_v );
    renderer->set3DTransferFunction( tfunc3d, width, height, depth );
    renderer->disableShading();
    
    screen.registerObject( volume_s, renderer );
    screen.show();
    
    return( app.run() );   
}