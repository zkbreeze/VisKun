/*****************************************************************************/
/**
 *  @file   main.cpp
 *  @author Naohisa Sakamoto
 *  @brief  Example program for kvs::glew::StochasticTetrahedraRenderer class.
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
#include <GL/glew.h>
#include <kvs/glut/Application>
#include <kvs/glut/Screen>
#include <kvs/PolygonObject>
#include <kvs/HydrogenVolumeData>
#include <kvs/Isosurface>
#include "StochasticStructuredVolumeEngine.h"
#include <kvs/glew/StochasticRenderingCompositor>


/*===========================================================================*/
/**
 *  @brief  Main function.
 *  @param  argc [i] argument count
 *  @param  argv [i] argument values
 */
/*===========================================================================*/
int main( int argc, char** argv )
{
    try{
    kvs::glut::Application app( argc, argv );

    kvs::PolygonObject* polygon_object = NULL;
    kvs::StructuredVolumeObject* volume = new kvs::HydrogenVolumeData( kvs::Vector3ui( 32, 32, 32 ) );
    volume->updateMinMaxValues();
    const kvs::TransferFunction t( 256 );

    kvs::glew::StochasticStructuredVolumeEngine* engine = new kvs::glew::StochasticStructuredVolumeEngine();
    engine->test_setVolume(volume);

    kvs::glut::Screen screen( &app );
    screen.show();
    screen.light()->setModelTwoSide( true );

    kvs::glew::StochasticRenderingCompositor compositor( &screen );
    compositor.setRepetitionLevel( 20 );
    compositor.enableLODControl( 1 );
    compositor.registerObject( volume, engine );

    return app.run();
    }
    catch ( std::exception& err )
    {
        std::cout << err.what() << std::endl;
        exit(EXIT_FAILURE);
    }
}
