#include <kvs/glut/Application>
#include <kvs/glut/Screen>
#include <kvs/glut/Timer>

#include <kvs/StructuredVolumeImporter>
#include <kvs/PolygonObject>
#include <kvs/Isosurface>
#include <kvs/TransferFunction>
#include <kvs/Shader>
#include <kvs/glew/StochasticUniformGridEngine>
#include <kvs/glew/StochasticPolygonEngine>
#include <kvs/glew/StochasticRenderingCompositor>
#include <kvs/ObjectManager>
#include <kvs/RendererManager>
#include <kvs/IDManager>
#include <kvs/RGBFormulae>
#include <kvs/ExternalFaces>
#include <kvs/Timer>
#include <kvs/TimerEventListener>
#include <kvs/KeyPressEventListener>
#include "WritePolygon.h"

kvs::Timer rendering_time;

class RenderingTime : public kvs::TimerEventListener
{
    void update( kvs::TimeEvent* event )
    {
        rendering_time.stop();
        std::cout << rendering_time.fps() << std::endl;
        screen()->redraw();
        rendering_time.start();
    }
};

class KeyPressEvent : public kvs::KeyPressEventListener
{
    void update( kvs::KeyEvent* event)
    {
        switch ( event->key() ) 
        {
            case kvs::Key::p:
            {
                std::cout << static_cast<kvs::glut::Screen*>( screen() )->camera()->position() << std::endl;
                std::cout << static_cast<kvs::glut::Screen*>( screen() )->camera()->upVector() << std::endl;
                break;
            }
            case kvs::Key::v:
            {

                static_cast<kvs::glut::Screen*>( screen() )->camera()->setPosition( kvs::Vector3f(-6.09234, -7.99483, 6.55486 ), kvs::Vector3f( 0, 0, 0 ), kvs::Vector3f( 0.352572, 0.417837, 0.83732));
                static_cast<kvs::glut::Screen*>( screen() )->redraw();
                std::cout << static_cast<kvs::glut::Screen*>( screen() )->camera()->position() << std::endl;
                std::cout << static_cast<kvs::glut::Screen*>( screen() )->camera()->upVector() << std::endl;
                std::cout << static_cast<kvs::glut::Screen*>( screen() )->camera()->lookAt() << std::endl;
//                static_cast<kvs::glut::Screen*>( screen() )->camera()->setUpVector(kvs::Vector3f( 0, 1, 0));
            }
            case kvs::Key::c:
            {
                static_cast<kvs::glut::Screen*>( screen() )->controlTarget() = kvs::Scene::TargetCamera;
                break;
            }
        }
    }
};

int main( int argc, char** argv )
{
    kvs::glut::Application app( argc, argv );
    kvs::glut::Screen screen( &app );
    screen.show();
    screen.light()->setModelTwoSide( true );
    screen.setBackgroundColor( kvs::RGBColor( 255, 255, 255 ));
    
//    RenderingTime time_fps;
//    kvs::glut::Timer* glut_timer = new kvs::glut::Timer( 1000 );
//    screen.addTimerEvent( &time_fps, glut_timer );
    
    KeyPressEvent key;
    screen.addKeyPressEvent( &key );
    
    // object 1
    kvs::StructuredVolumeObject* object1 = new kvs::StructuredVolumeImporter( argv[1]);
    float min = object1->minValue();
    float max = object1->maxValue() / 8.0;
    std::cout << "min value: " << min << std::endl;
    std::cout << "max value: " << max << std::endl;
    float* pvalues = (float*)object1->values().pointer();
    for ( size_t i = 0; i < object1->nnodes(); i++) {
        if( pvalues[i] < min ) pvalues[i] = min;
        if( pvalues[i] > max ) pvalues[i] = max;
    }
    object1->updateMinMaxValues();
    std::cout << object1->minValue() << std::endl;
    std::cout << object1->maxValue() << std::endl;
    
    kvs::glew::StochasticUniformGridEngine* engine1 = new kvs::glew::StochasticUniformGridEngine();
    engine1->setShader( kvs::Shader::BlinnPhong( 0.8, 0.2, 0.8, 100 ) );
    engine1->disableShading();
    kvs::TransferFunction tfunc1( 256 );
    kvs::OpacityMap omap1( 256 );
    omap1.setRange( object1->minValue(), object1->maxValue() );
    omap1.addPoint( object1->minValue(), 0 );
    omap1.addPoint( ( object1->maxValue() - object1->minValue() ) * 0.9 + object1->minValue(), 0.04 );
    omap1.addPoint( object1->maxValue(), 0 );
    omap1.create();
    kvs::OpacityMap omap2( omap1.table() );
    
    tfunc1.setOpacityMap( omap2 );
    tfunc1.write( "tfunc10.kvsml" );
//    tfunc1.setColorMap( kvs::RGBFormulae::GreenRedViolet( 256 ));
    engine1->setTransferFunction( tfunc1 );
    
//    // object 2
//    kvs::StructuredVolumeObject* object2 = new kvs::StructuredVolumeImporter( argv[3] );
//    kvs::glew::StochasticUniformGridEngine* engine2 = new kvs::glew::StochasticUniformGridEngine();
//    engine2->setShader( kvs::Shader::BlinnPhong( 0.8, 0.2, 0.8, 50 ) );
//    kvs::TransferFunction tfunc2( argv[4] );
//    engine2->setTransferFunction( tfunc2 );
    
    // isosurface
    kvs::StructuredVolumeObject* volume = new kvs::StructuredVolumeImporter( argv[2] );
    kvs::PolygonObject::NormalType n = kvs::PolygonObject::VertexNormal;
    bool d = false;
    kvs::TransferFunction t( 256 );
    kvs::PolygonObject* object2 = new kvs::Isosurface( volume, 0.1, n, d, t );
    object2->setOpacity( 100 );
//    object2->setColor( kvs::TransferFunction( 256 ).colorMap()[ int ( 256 * 0.1 )]);
    float grey = 0.4;
    object2->setColor( kvs::RGBColor( grey * 255, grey * 255, 0.6 * 255 ));
//    WritePolygon( object2, "pressure10.kvsml" );
    kvs::glew::StochasticPolygonEngine* engine2 = new kvs::glew::StochasticPolygonEngine();
    engine2->setShader( kvs::Shader::BlinnPhong( 0.8, 0.2, 0.8, 100 ) );
    
    kvs::ObjectManager* object_manager = screen.objectManager();
    kvs::RendererManager* renderer_manager = screen.rendererManager();
    kvs::IDManager* id_manager = screen.IDManager();
    kvs::glew::StochasticRenderingCompositor compositor( object_manager, renderer_manager, id_manager );
    compositor.setRepetitionLevel( 30 );
    compositor.enableLODControl( 1 );
    compositor.registerObject( object1, engine1 );
    compositor.registerObject( object2, engine2 );
    compositor.disableLODControl();
    
//    glut_timer->start( 10 );

    return app.run();
}
