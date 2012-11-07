#include <kvs/glut/Application>
#include <kvs/glut/Screen>

#include <kvs/HydrogenVolumeData>
#include <kvs/TornadoVolumeData>
#include <kvs/StructuredVolumeObject>
#include <kvs/StructuredVectorToScalar>
#include <kvs/TransferFunction>
#include <kvs/ColorMap>
#include <kvs/OpacityMap>
#include <kvs/RGBFormulae>
#include <kvs/Shader>
#include <kvs/glew/StochasticUniformGridEngine>
#include <kvs/glew/StochasticRenderingCompositor>
#include <kvs/ObjectManager>
#include <kvs/RendererManager>
#include <kvs/IDManager>


int main( int argc, char** argv )
{
    kvs::glut::Application app( argc, argv );
    kvs::glut::Screen screen( &app );
    screen.background()->setColor( kvs::RGBColor( 255, 255, 255 ) );
    screen.show();

    kvs::Vector3ui resolution( 32, 32, 32 );
    kvs::StructuredVolumeObject* object1 = new kvs::HydrogenVolumeData( resolution );
    kvs::glew::StochasticUniformGridEngine* engine1 = new kvs::glew::StochasticUniformGridEngine();
    engine1->setShader( kvs::Shader::BlinnPhong( 0.3, 0.5, 0.8, 100 ) );
    kvs::ColorMap cmap1( kvs::RGBFormulae::Hot( 256 ) );
    kvs::OpacityMap omap1( 256 );
    omap1.setRange( object1->minValue(), object1->maxValue() );
    omap1.addPoint( object1->minValue(), 0 );
    omap1.addPoint( object1->maxValue(), 0.8 );
    omap1.create();
    kvs::TransferFunction tfunc1( cmap1, omap1 );
    engine1->setTransferFunction( tfunc1 );

    kvs::StructuredVolumeObject* volume = new kvs::TornadoVolumeData( resolution );
    kvs::StructuredVolumeObject* object2 = new kvs::StructuredVectorToScalar( volume );
    kvs::glew::StochasticUniformGridEngine* engine2 = new kvs::glew::StochasticUniformGridEngine();
    engine2->setShader( kvs::Shader::BlinnPhong( 0.8, 0.2, 0.8, 50 ) );
    kvs::ColorMap cmap2( kvs::RGBFormulae::PM3D( 256 ) );
    kvs::OpacityMap omap2( 256 );
    omap2.setRange( object2->minValue(), object2->maxValue() );
    omap2.addPoint( object2->minValue(), 0 );
    omap2.addPoint( object2->maxValue(), 0.3 );
    omap2.create();
    kvs::TransferFunction tfunc2( cmap2, omap2 );
    engine2->setTransferFunction( tfunc2 );
    delete volume;

    kvs::ObjectManager* object_manager = screen.objectManager();
    kvs::RendererManager* renderer_manager = screen.rendererManager();
    kvs::IDManager* id_manager = screen.IDManager();
    kvs::glew::StochasticRenderingCompositor compositor( object_manager, renderer_manager, id_manager );
    compositor.setRepetitionLevel( 50 );
    compositor.enableLODControl( 1 );
    compositor.registerObject( object1, engine1 );
    compositor.registerObject( object2, engine2 );

    return app.run();
}
