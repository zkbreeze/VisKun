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

using namespace std;
kvs::TransferFunction tf;

class Argument : public kvs::CommandLine
{
public:
    
    std::string filename_s;
    std::string filename_t;
    kvs::TransferFunction tfunc;
    std::string outname;
    size_t grid_number;
    
    Argument( int argc, char** argv ) : CommandLine ( argc, argv )
    {
        add_help_option();
        addOption( "s", "filename of s", 1, true );
        addOption( "t", "filename of t", 1, true );
        addOption( "tfunc", "tfunc", 1, false );
        addOption( "outname", "output filename", 1, false );
        addOption( "g", "grid number", 1, false );
    }
    
    void exec()
    {        
        tfunc.create( 256 );
        if( !this->parse() ) exit( EXIT_FAILURE );
        if( this->hasOption( "s" ) ) filename_s = this->optionValue<std::string>( "s" );
        if( this->hasOption( "t" ) ) filename_t = this->optionValue<std::string>( "t" );
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

kvs::StructuredVolumeObject* ValueProcessing( kvs::StructuredVolumeObject* object_x, kvs::StructuredVolumeObject* object_y, size_t grid_x, size_t grid_y )
{
    size_t nx_ori = object_x->resolution().x();
    size_t ny_ori = object_x->resolution().y();
    size_t nz_ori = object_x->resolution().z();

    float* pvalues_x = (float*)object_x->values().pointer();
    float* pvalues_y = (float*)object_y->values().pointer();
    float range_x = object_x->maxValue() - object_x->minValue();
    float range_y = object_y->maxValue() - object_y->minValue();
    float scale_x = (float)grid_x / range_x;
    float scale_y = (float)grid_y / range_y;
    
    // value processing
    unsigned int n = object_y->nnodes();
    kvs::AnyValueArray values;
    float* pvalues = static_cast<float*>( values.allocate<float>( n ) );
    size_t grid_number = grid_x * grid_y;
    size_t* grid = new size_t[grid_number];
    for ( size_t i = 0; i < grid_number; i++ ) grid[i] = 0;
    size_t max = 0;
    for ( size_t i = 0; i < n ; i++ )
    {
        int index_x = (int)(( pvalues_x[i] - object_x->minValue() ) * scale_x );
        int index_y = (int)(( pvalues_y[i] - object_y->minValue() ) * scale_y );
        if ( pvalues_x[i] == object_x->maxValue() ) index_x = grid_x - 1;
        if ( pvalues_y[i] == object_y->maxValue() ) index_y = grid_y - 1;
        int index = index_y + index_x * grid_y;
        pvalues[i] = (float)index;
        grid[index]++;
        if ( grid[index] > max ) max = grid[index];
    }
    std::cout << "max grid number: " << max << std::endl;
    
    // set transfer function
    kvs::ColorMap cmap( grid_number );
//    kvs::ColorMap ref_cmap( grid_x );
//    ref_cmap.create();
    cmap.setRange( 0, grid_number - 1 );
//    for ( size_t i = 0; i < ( grid_x - 1 ); i++ )
//    {
//        cmap.addPoint( ( i + 1 ) * grid_x + 0.25, ref_cmap.at( i ) );
//        cmap.addPoint( ( i + 1 ) * grid_x + 0.75, ref_cmap.at( i + 1 ));
//    }
    cmap.addPoint( 0, kvs::RGBColor( 0, 0, 255 ) );
    cmap.addPoint( 0.25, kvs::RGBColor( 0, 0, 255 ) );
    
    // water
    cmap.addPoint( 0.5, kvs::RGBColor( 0, 255, 255 ) );
    cmap.addPoint( ( 32 - object_x->minValue() ) * scale_x * grid_y, kvs::RGBColor( 0, 255, 255 ) );
    
    // blue
    cmap.addPoint( ( 32 - object_x->minValue() ) * scale_x * grid_y + 0.5, kvs::RGBColor( 0, 0, 255 ) );
    cmap.addPoint( ( 33.3 - object_x->minValue() ) * scale_x * grid_y, kvs::RGBColor( 0, 0, 255 ) );
    
    // yellow
    cmap.addPoint( ( 33.3 - object_x->minValue() ) * scale_x * grid_y + 0.5, kvs::RGBColor( 255, 255, 0 ) );
    cmap.addPoint( ( 33.6 - object_x->minValue() ) * scale_x * grid_y, kvs::RGBColor( 255, 255, 0 ) );
    
    //TW
    for ( size_t i = 0; i < ( 34.5 - 33.6 ) * scale_x; i++ )
    {
        // yellow
        cmap.addPoint( (( 33.6 - object_x->minValue() ) * scale_x + i ) * grid_y + 0.5, kvs::RGBColor( 255, 255, 0 ) );
        cmap.addPoint( (( 33.6 - object_x->minValue() ) * scale_x + i ) * grid_y + (3 - object_y->minValue()) * scale_y, kvs::RGBColor( 255, 255, 0 ) );
        // TW green
        cmap.addPoint( (( 33.6 - object_x->minValue() ) * scale_x + i ) * grid_y + (3 - object_y->minValue()) * scale_y + 0.5, kvs::RGBColor( 0, 255, 0 ) );
        cmap.addPoint( (( 33.6 - object_x->minValue() ) * scale_x + i ) * grid_y + object_y->maxValue() * scale_y, kvs::RGBColor( 0, 255, 0 ) );
    }
    
    // KW red
    for ( size_t i = 0; i < ( object_x->maxValue() - 34.5 ) * scale_x; i++ )
    {
        // yellow
        cmap.addPoint( (( 34.5 - object_x->minValue() ) * scale_x + i ) * grid_y + 0.5, kvs::RGBColor( 255, 255, 0 ) );
        cmap.addPoint( (( 34.5 - object_x->minValue() ) * scale_x + i ) * grid_y + (3 - object_y->minValue()) * scale_y, kvs::RGBColor( 255, 255, 0 ) );
        // KW red
        cmap.addPoint( (( 34.5 - object_x->minValue() ) * scale_x + i ) * grid_y + (3 - object_y->minValue()) * scale_y + 0.5, kvs::RGBColor( 255, 0, 0 ) );
        cmap.addPoint( (( 33.6 - object_x->minValue() ) * scale_x + i ) * grid_y + object_y->maxValue() * scale_y, kvs::RGBColor( 255, 0, 0 ) );
    }
//
//    
//    // red
//    cmap.addPoint( ( 33.6 - object_x->minValue() ) * scale_x * grid_y + 0.5, kvs::RGBColor( 255, 0, 0 ) );
//    cmap.addPoint( grid_number - 1, kvs::RGBColor( 255, 0, 0 ) );
//    cmap.create();
    
    std::cout << "succeed in creating cmap" << std::endl;
    
    kvs::OpacityMap omap( grid_number );
    omap.setRange( 0, grid_number );
    omap.addPoint( 0, 0 );
    for ( size_t i = 0; i < grid_number - 1; i++ )
    {
        omap.addPoint( i + 0.25, (float)grid[i] / max );
        omap.addPoint( i + 0.75, (float)grid[i + 1] / max );
    }
    omap.addPoint( grid_number, 0 );
    omap.create();
    std::cout << "succeed in creating omap" << std::endl;
    
    tf.create( grid_number );
    tf.setRange( 0, grid_number - 1 );
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
    kvs::StructuredVolumeObject* volume_t = new kvs::StructuredVolumeImporter( param.filename_t );
    kvs::StructuredVolumeObject* volume = ValueProcessing( volume_s, volume_t, param.grid_number, param.grid_number );
    std::cout << *volume << std::endl;
    if( param.hasOption( "outname" ) )
        WriteKVSML( volume, param.outname );
    
    kvs::glew::RayCastingRenderer* renderer
    = new kvs::glew::RayCastingRenderer();
    renderer->disableShading();
    
    TransferFunctionEditor editor( &screen );
    editor.setTransferFunction( tf );
    renderer->setTransferFunction( editor.transferFunction() );
    editor.setVolumeObject( volume );
    editor.show();
    
    screen.registerObject( volume, renderer );
    
    return( app.run() );
}
