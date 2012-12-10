//
//  main.cpp
//  
//
//  Created by Kun Zhao on 12/11/30.
//  Copyright (c) 2012 Kyoto University. All rights reserved.
//

#include <iostream>
#include <fstream>
#include <kvs/glut/Application>
#include <kvs/glut/Screen>
#include <kvs/StructuredVolumeObject>
#include <kvs/StructuredVolumeImporter>
#include <kvs/UnstructuredVolumeObject>
#include <kvs/LineObject>
#include <kvs/PointObject>
#include <kvs/CellByCellMetropolisSampling>
#include <kvs/glew/ParticleVolumeRenderer>
#include <kvs/glew/RayCastingRenderer>
#include <kvs/KVSMLObjectUnstructuredVolume>
#include <kvs/UnstructuredVolumeExporter>
#include <kvs/TransferFunction>
#include <kvs/glut/TransferFunctionEditor>
#include <kvs/ExtractEdges>
#include <kvs/ExtractVertices>
#include <kvs/CommandLine>
#include <kvs/Timer>
#include <kvs/KVSMLObjectStructuredVolume>
#include <kvs/StructuredVolumeExporter>
#include "float.h"

using namespace std;
kvs::TransferFunction tf;

class Argument : public kvs::CommandLine
{
public:
    
    std::string filename_s;
    std::string filename_u;
    std::string filename_v;
    std::string filename_w;
    kvs::TransferFunction tfunc;
    std::string outname;
    size_t grid_number;
    
    Argument( int argc, char** argv ) : CommandLine ( argc, argv )
    {
        add_help_option();
        addOption( "s", "filename of s", 1, true );
        addOption( "u", "filename of u", 1, true );
        addOption( "v", "filename of v", 1, true );
        addOption( "w", "filename of w", 1, true );
        addOption( "tfunc", "tfunc", 1, false );
        addOption( "outname", "output filename", 1, false );
        addOption( "g", "grid number", 1, false );
    }
    
    void exec()
    {        
        tfunc.create( 256 );
        if( !this->parse() ) exit( EXIT_FAILURE );
        if( this->hasOption( "s" ) ) filename_s = this->optionValue<std::string>( "s" );
        if( this->hasOption( "u" ) ) filename_u = this->optionValue<std::string>( "u" );
        if( this->hasOption( "v" ) ) filename_v = this->optionValue<std::string>( "v" );
        if( this->hasOption( "w" ) ) filename_w = this->optionValue<std::string>( "w" );
        if( this->hasOption( "tfunc" ) ) tfunc = kvs::TransferFunction( this->optionValue<std::string>( "tfunc" ) );
        if( this->hasOption( "outname" ) ) outname = this->optionValue<std::string>( "outname" );
        if( this->hasOption( "g" ) ) grid_number = this->optionValue<size_t>( "g" );
    }
};

class TransferFunctionEditor : public kvs::glut::TransferFunctionEditor
{
    
public:
    
    TransferFunctionEditor( kvs::ScreenBase* screen ) :
    kvs::glut::TransferFunctionEditor( screen )
    {
    }
    
    void apply( void )
    {
        kvs::RendererBase* r = screen()->rendererManager()->renderer();
        kvs::glew::RayCastingRenderer* renderer = static_cast<kvs::glew::RayCastingRenderer*>( r );
        renderer->setTransferFunction( transferFunction() );
        screen()->redraw();
    }
    
};

kvs::StructuredVolumeObject* ValueProcessing( 
                                             kvs::StructuredVolumeObject* object_x, 
                                             kvs::StructuredVolumeObject* object_u,
                                             kvs::StructuredVolumeObject* object_v, 
                                             kvs::StructuredVolumeObject* object_w, 
                                             size_t grid_y 
                                             )
{
    size_t nx_ori = object_x->resolution().x();
    size_t ny_ori = object_x->resolution().y();
    size_t nz_ori = object_x->resolution().z();

    float* pvalues_x = (float*)object_x->values().pointer();
    float* pvalues_u = (float*)object_u->values().pointer();
    float* pvalues_v = (float*)object_v->values().pointer();
    float* pvalues_w = (float*)object_w->values().pointer();
    unsigned int n = object_x->nnodes();
    
    // calculate the absolute value of velocity
    float* pvalues_y = new float[n];
    float min = FLT_MAX;
    float max = -FLT_MAX;
    for ( size_t i = 0; i < n; i++ )
    {
        pvalues_y[i] = std::sqrt( pvalues_u[i] * pvalues_u[i] + pvalues_v[i] * pvalues_v[i] + pvalues_w[i] * pvalues_w[i] );
        if ( pvalues_y[i] > max ) max = pvalues_y[i];
        if ( pvalues_y[i] < min ) min = pvalues_y[i];
    }
    std::cout << "min value of velocity: " << min << std::endl;
    std::cout << "max value of velocity: " << max << std::endl;
    float scale_y = (float)grid_y / ( max - min );
    
    float x1 = 32;
    float x2 = 33.3;
    float x3 = 33.6;
    float x4 = 34.5;
        
    // value processing
    kvs::AnyValueArray values;
    float* pvalues = static_cast<float*>( values.allocate<float>( n ) );
    size_t grid_number = 5 * grid_y;

    for ( size_t i = 0; i < n ; i++ )
    {
        // index_x
        int index_x;
        if ( object_x->minValue() <= pvalues_x[i] && pvalues_x[i] < x1 ) index_x = 0;
        if ( x1 <= pvalues_x[i] && pvalues_x[i] < x2 ) index_x = 1;
        if ( x2 <= pvalues_x[i] && pvalues_x[i] < x3 ) index_x = 2;
        if ( x3 <= pvalues_x[i] && pvalues_x[i] < x4 ) index_x = 3;
        if ( x4 <= pvalues_x[i] && pvalues_x[i] <= object_x->maxValue() ) index_x = 4;
        
        // index_y
        int index_y = (int)(( pvalues_y[i] - min ) * scale_y );
        if ( pvalues_y[i] == max ) index_y = grid_y - 1;
        int index = index_y + index_x * grid_y;
        pvalues[i] = (float)index;
    }
    
    // set transfer function
    kvs::ColorMap cmap( grid_number );
    cmap.setRange( 0, grid_number - 1 );

    cmap.addPoint( 0, kvs::RGBColor( 0, 0, 255 ) );
    cmap.addPoint( 0.25, kvs::RGBColor( 0, 0, 255 ) );
    
    // water
    cmap.addPoint( 0.5, kvs::RGBColor( 0, 255, 255 ) );
    cmap.addPoint( grid_y - 1, kvs::RGBColor( 0, 255, 255 ) );
    
    // blue
    cmap.addPoint( grid_y - 0.5, kvs::RGBColor( 0, 0, 255 ) );
    cmap.addPoint( 2 * grid_y - 1, kvs::RGBColor( 0, 0, 255 ) );
    
    // yellow
    cmap.addPoint( 2 * grid_y - 0.5, kvs::RGBColor( 255, 255, 0 ) );
    cmap.addPoint( 3 * grid_y - 1, kvs::RGBColor( 255, 255, 0 ) );
    
    // green
    cmap.addPoint( 3 * grid_y - 0.5, kvs::RGBColor( 0, 255, 0 ) );
    cmap.addPoint( 4 * grid_y - 1, kvs::RGBColor( 0, 255, 0 ) );
    
    // red
    cmap.addPoint( 4 * grid_y - 0.5, kvs::RGBColor( 255, 0, 0 ) );
    cmap.addPoint( grid_number - 1, kvs::RGBColor( 255, 0, 0 ) );
    cmap.create();
    
    std::cout << "succeed in creating cmap" << std::endl;
    
    kvs::OpacityMap omap( grid_number );
    omap.setRange( 0, grid_number );
    omap.addPoint( 0, 0 );
    omap.addPoint( 0.1, 0);
    float omap_scale = 4;
    for ( size_t i = 1; i < grid_number - 1; i++ )
    {
        omap.addPoint( i + 0.25, (float)( i % grid_y ) / ( grid_y * omap_scale ) );
        omap.addPoint( i + 0.75, (float)(( i + 1 ) % grid_y) / ( grid_y * omap_scale ) );
    }
    omap.addPoint( grid_number, 1 );
    omap.create();
    std::cout << "succeed in creating omap" << std::endl;
    
    tf.create( grid_number );
    tf.setRange( 0, grid_number );
    tf.setColorMap( cmap );
    tf.setOpacityMap( omap );
        
    kvs::Vector3ui resolution( nx_ori, ny_ori, nz_ori );
    kvs::VolumeObjectBase::GridType grid_type = kvs::VolumeObjectBase::Uniform;
    kvs::StructuredVolumeObject* object = new kvs::StructuredVolumeObject();
    object->setGridType( grid_type);
    object->setVeclen( 1 );
    object->setResolution( resolution );
    object->setValues( values );
    
    object->updateMinMaxCoords();
    object->updateMinMaxValues();
    
    return ( object );
}

void WriteKVSML( kvs::StructuredVolumeObject* object, std::string filename )
{
    kvs::KVSMLObjectStructuredVolume* kvsml = new kvs::StructuredVolumeExporter<kvs::KVSMLObjectStructuredVolume>( object );
    kvsml->setWritingDataType( kvs::KVSMLObjectStructuredVolume::ExternalBinary );
    kvsml->write( filename );
}


int main( int argc, char** argv )
{
    kvs::glut::Application app( argc, argv );
    kvs::glut::Screen screen( &app );
    screen.show();

    Argument param( argc, argv );
    param.exec();
    
    //Load Volume Data
    kvs::StructuredVolumeObject* volume_s = new kvs::StructuredVolumeImporter( param.filename_s );
    kvs::StructuredVolumeObject* volume_u = new kvs::StructuredVolumeImporter( param.filename_u );
    kvs::StructuredVolumeObject* volume_v = new kvs::StructuredVolumeImporter( param.filename_v );
    kvs::StructuredVolumeObject* volume_w = new kvs::StructuredVolumeImporter( param.filename_w );
    kvs::StructuredVolumeObject* volume = ValueProcessing( volume_s, volume_u, volume_v, volume_w, param.grid_number );
    std::cout << *volume << std::endl;
    if( param.hasOption( "outname" ) )
        WriteKVSML( volume, param.outname );
    
    kvs::glew::RayCastingRenderer* renderer
    = new kvs::glew::RayCastingRenderer();
    renderer->disableShading();
    
//    TransferFunctionEditor editor( &screen );
//    editor.setTransferFunction( tf );
//    renderer->setTransferFunction( editor.transferFunction() );
//    editor.setVolumeObject( volume );
//    editor.show();
    renderer->setTransferFunction( tf );
    
    screen.registerObject( volume, renderer );

    
    return( app.run() );
}
