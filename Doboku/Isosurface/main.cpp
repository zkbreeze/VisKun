//
//  main.cpp
//  
//
//  Created by Kun Zhao on 12/10/04.
//  Copyright (c) 2012 Kyoto University. All rights reserved.
//

#include <iostream>
#include <kvs/glut/Application>
#include <kvs/glut/Screen>
#include <kvs/StructuredVolumeObject>
#include <kvs/StructuredVolumeImporter>
#include <kvs/Isosurface>
#include <kvs/Bounds>
#include <kvs/TimerEventListener>
#include <kvs/ExtractEdges>
#include <kvs/LineObject>

//namespace { kvs::StructuredVolumeObject* Volume = NULL; }
//namespace { double Isolevel = 255.0; }
//namespace { double Isolevel = 10; }

kvs::glut::Timer* timer;
double Isolevel = 1.0;

class UpdateIsolevel : public kvs::TimerEventListener
{
    kvs::PolygonObject::NormalType m_norm;
    bool m_dup;
    kvs::TransferFunction m_tfunc;
    kvs::StructuredVolumeObject* m_volume;
    std::string m_objectname;
    
public:
    
    UpdateIsolevel( kvs::PolygonObject::NormalType norm, bool dup, kvs::TransferFunction tfunc, kvs::StructuredVolumeObject* volume, std::string objectname ):
    m_norm( norm ),
    m_dup( dup ),
    m_tfunc( tfunc ),
    m_volume( volume ),
    m_objectname( objectname ){}
    
    void update( kvs::TimeEvent* event )
    {
        std::cout << "                       " << "\r" << std::flush;
        std::cout << "isolevel: " << Isolevel << "\r" << std::flush;
        if( Isolevel > 0 ) 
            Isolevel -= 0.02;
        else
            Isolevel = 1.0;
        
        kvs::PolygonObject* object = new kvs::Isosurface( m_volume, Isolevel, m_norm, m_dup, m_tfunc );
        object->setName( m_objectname );
        
        screen()->objectManager()->change( m_objectname, object );
        screen()->redraw();
    }
};

class KeyPressEvent : public kvs::KeyPressEventListener
{
    void update( kvs::KeyEvent* event )
    {
        switch ( event->key() ) 
        {
            case kvs::Key::s:
            {
                if( timer->isStopped() ) timer->start();
                else timer->stop();
                break;
            }
        }
    }
};

int main( int argc, char** argv )
{
    kvs::glut::Application app( argc, argv );
    kvs::StructuredVolumeObject* volume = new kvs::StructuredVolumeImporter( argv[1] );
    std::cout << *volume << std::endl;
    
    kvs::PolygonObject::NormalType n = kvs::PolygonObject::VertexNormal;
    bool d = false;
    kvs::TransferFunction t( 256 );
    kvs::PolygonObject* object = new kvs::Isosurface( volume, Isolevel, n, d, t );
    object->setName("Isosurface");
    
    kvs::LineObject* bounds = new kvs::Bounds( volume );
    
    int msec = 100;
    timer = new kvs::glut::Timer( msec );
    
    UpdateIsolevel update_isolevel( n, d, t, volume, "Isosurface" );
    KeyPressEvent keypressevent;
    
    kvs::glut::Screen screen( &app );
    screen.registerObject( object );
    screen.registerObject( bounds );
    screen.addTimerEvent( &update_isolevel, timer );
    screen.addKeyPressEvent( &keypressevent );
    screen.show(); 
    
    return app.run();
}