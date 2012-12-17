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
    float* tfunc2d = new float[width * height * 4];
    for ( size_t j = 0; j < height; j++ )
        for ( size_t i = 0; i < width; i++ )
        {
            int index = ( i + j * width ) * 4;
            tfunc2d[index] = (float)i / width; // red
            tfunc2d[index + 1] = (float)j / height; // green
            tfunc2d[index + 2] = 1;              // blue
            tfunc2d[index + 3] = (float) i * j / ( width * height ); //alpha
//            *(tfunc2d++) = 0.01;
        }
    
    kvs::glew::kun::RayCastingRenderer* renderer = new kvs::glew::kun::RayCastingRenderer();
    
    renderer->addVolume( volume_t );
    renderer->set2DTransferFunction( tfunc2d, width, height );
    renderer->disableShading();
    
    screen.registerObject( volume_s, renderer );
    screen.show();
    
    return( app.run() );   
}