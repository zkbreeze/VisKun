q//
//  main.c
//  
//
//  Created by コン チョウ on 11/12/15.
//  Copyright (c) 2011年 __MyCompanyName__. All rights reserved.
//
#include <iostream>
#include <cstdio>
#include <cmath>
#include <kvs/Matrix33> 

using namespace std;

//set the value of coordinate and tensor for the vertex of one cell　kは点の番号、立法格子の中心は原点とする
//coordinate
double a[8] = { -1, 1, 1, -1, -1, 1, 1, -1 };
double b[8] = { -1, -1, 1, 1, -1, -1, 1, 1 };
double c[8] = { -1, -1, -1, -1, 1, 1, 1, 1 }; 

//set the value of tensor of every vertex 
kvs::Matrix33<double> T[8] = { 
    //vertex 0 
    kvs::Matrix33<double>(
    3.333333333,    0.333333333,    1.333333333,
    0.333333333,    3.333333333,    1.333333333,
    1.333333333,    1.333333333,    2.333333333),
    //vertex 1
    kvs::Matrix33<double>(
    3.333333333,    -0.333333333,   -1.333333333,
    -0.333333333,   3.333333333,    1.333333333,
    -1.333333333,   1.333333333,    2.333333333),
    //vertex 2
    kvs::Matrix33<double>(
    3.333333333,    0.333333333,    -1.333333333,
    0.333333333,    3.333333333,    -1.333333333,
    -1.333333333,   -1.333333333,   2.333333333),
    //vertex 3
    kvs::Matrix33<double>(
    3.333333333,    -0.333333333,   1.333333333,
    -0.333333333,   3.333333333,    -1.333333333,
    1.333333333,    -1.333333333,   2.333333333),
    //vertex 4
    kvs::Matrix33<double>(
    3.333333333,	0.333333333,	-1.333333333,
    0.333333333,	3.333333333,	-1.333333333,
    -1.333333333,	-1.333333333,	2.333333333),
    //vertex 5
    kvs::Matrix33<double>(
    3.333333333,	-0.333333333,	1.333333333,
    -0.333333333,	3.333333333,	-1.333333333,
    1.333333333,	-1.333333333,	2.333333333),
    //vertex 6
    kvs::Matrix33<double>(
    3.333333333,	0.333333333,	1.333333333,
    0.333333333,	3.333333333,	1.333333333,
    1.333333333,	1.333333333,	2.333333333),
    //vertex 7
    kvs::Matrix33<double>(
    3.333333333,	-0.333333333,	-1.333333333,
    -0.333333333,	3.333333333,	1.333333333,
    -1.333333333,	1.333333333,	2.333333333) 
//    //vertex 0 
//    kvs::Matrix33<double>(
//                          3.12638315,	0.19572038,	1.059621921,
//                          0.19572038,	3.303097898,	1.640959297,
//                          1.059621921,	1.640959297,	2.570518951),
//    //vertex 1
//    kvs::Matrix33<double>(
//                          2.968585792,	-0.071235146,	0.803235289,
//                          -0.071235146,	2.838466531,	1.821423692,
//                          0.803235289,	1.821423692,	3.192947678),
//    //vertex 2
//    kvs::Matrix33<double>(
//                          2.652910755,	-0.088356265,	1.904806311,
//                          -0.088356265,	2.977507717,	0.484894228,
//                          1.904806311,	0.484894228,	3.369581528),
//    //vertex 3
//    kvs::Matrix33<double>(
//                          2.956675805,	-0.16823867,	0.467526175,
//                          -0.16823867,	2.346687219,	1.815520917,
//                          0.467526175,	1.815520917,	3.696636976),
//    //vertex 4
//    kvs::Matrix33<double>(
//                          3.005245845,	0.031797101,	0.323957653,
//                          0.031797101,	3.192734581,	1.963633161,
//                          0.323957653,	1.963633161,	2.802019574),
//    //vertex 5
//    kvs::Matrix33<double>(
//                          3.273698656,	0.14739751,	1.733227877,
//                          0.14739751,	3.079379367,	0.933411504,
//                          1.733227877,	0.933411504,	2.646921977),
//    //vertex 6
//    kvs::Matrix33<double>(
//                          3.587152919,	0.4975417,	1.317543373,
//                          0.4975417,	3.421606936,	1.116460037,
//                          1.317543373,	1.116460037,	1.991240144),
//    //vertex 7
//    kvs::Matrix33<double>(
//                          2.933332985,	-0.306151761,	0.287947918,
//                          -0.306151761,	1.594073843,	1.322329515,
//                          0.287947918,	1.322329515,	4.472593173)    
};

//the function of P
double func_P( const kvs::Matrix33<double> &a )
{
    double sum = 0.0;
    int i;
    for (i = 0 ; i < 3; i ++) {
        sum += a[i][i];
    }
    return( sum );
}

//the function of Q
double func_Q( const kvs::Matrix33<double> &a )
{
    double sum = 0.0;
    int i, j;
    for (i = 0; i < 3; i ++) {
        j = (i + 1) % 3; 
        sum += a[i][i] * a[j][j] - a[i][j] * a[j][i];
    } 
    return( sum );
}

//the function of R
double func_R( const kvs::Matrix33<double> &a )
{
    return( a.determinant() );
}


//the interpolation function of calculating the tensor value of some point in the cube
void InterTensor(double x, double y, double z, double a[8], double b[8], double c[8], kvs::Matrix33<double> T[8], kvs::Matrix33<double> &t )
{

    int i, j, k;
    for(i = 0; i < 3 ; i ++)
    {
        for(j = 0; j < 3 ; j ++)
        {
            t[i][j] = 0.0;
            for(k = 0; k <8; k ++) 
                t[i][j] +=  0.125 * ( 1 + x * a[k])*( 1 + y * b[k]) * ( 1+ z * c[k] ) * T[k][i][j];
        }
    }
}

//the discriminant of D3
double Disc_D(double P, double Q, double R)
{
    double D;
    D = Q*Q*P*P-4*R*P*P*P-4*Q*Q*Q+18*P*Q*R-27*R*R;
    return( D );
}

//the discriminant of Da
double Disc_Da(double P, double Q, double R)
{
    double A;
    A = -2.0/27*P*P*P+1.0/3*P*Q-R;
    return( A );
}

double f(double x,double y,double z )
{
    kvs::Matrix33<double> t;
    InterTensor(x, y, z, a, b, c, T, t);
    return( Disc_D( func_P( t ), func_Q( t ), func_R( t ) ) );
    
}
           
double g(double x,double y,double z )
{
    kvs::Matrix33<double> t;
    InterTensor(x, y, z, a, b, c, T, t);
    return( Disc_Da( func_P( t ), func_Q( t ), func_R( t ) ) );
}

/*
int main( )
{  
    //assume the coordanite of some point in the cube is x, y, z
    double x, y, z;
    
    //make a test at point(0, 0, 0)
    cout<<f(0, 0, 0)<<endl;
    
    //the discriminant D3, this discriminant should be equal to 0
//    f(x, y, z) == 0;
    
    //the discriminant Da, this discriminant should be equal to 0 or larger than 0
//    g(x, y, z) >= 0;
    
    //Draw the picture, 定義領域:x is from -1 to 1, y is from -1 to 1, z is from -1 to 1
    
    
    
    
} 
*/



#include <kvs/glut/Application>
#include <kvs/glut/Screen>
#include <kvs/StructuredVolumeObject>
#include <kvs/Isosurface>
#include <kvs/Bounds>
#include <kvs/TimerEventListener>
#include <kvs/ExtractEdges>
#include <kvs/LineObject>

namespace { kvs::StructuredVolumeObject* Volume = NULL; }
namespace { kvs::StructuredVolumeObject* Volume2 = NULL; }
namespace { double Isolevel = 255.0; }
//namespace { double Isolevel = 10; }

kvs::glut::Timer* timer;
kvs::glut::Timer* timer2;


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
        m_tfunc( tfunc ) ,
        m_volume( volume ),
        m_objectname( objectname ){}

    void update( kvs::TimeEvent* event )
    {
        std::cout << "                       " << "\r" << std::flush;
        std::cout << "isolevel: " << ::Isolevel << "\r" << std::flush;

        if ( 100.0 <= ::Isolevel && ::Isolevel <= 255.0 ) ::Isolevel -= 10.0;
        else if ( 10.0 < ::Isolevel && ::Isolevel <= 100.0 ) ::Isolevel -= 5.0;
        else if ( 1.0 < ::Isolevel && ::Isolevel <= 10.0 ) ::Isolevel -= 1.0;
        else if ( 1.0e-10 < ::Isolevel && ::Isolevel <= 1.0 ) ::Isolevel /= 2.0;
        else ::Isolevel = 255.0;

        kvs::PolygonObject* object = new kvs::Isosurface( m_volume, ::Isolevel, m_norm, m_dup, m_tfunc );
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
            case kvs::Key::d:
            {
                if( timer2->isStopped() ) timer2->start();
                else timer2->stop();
                break;
            }
            case kvs::Key::a:
            {
                if( timer->isStopped() && timer2->isStopped() )
                {
                    timer->start();
                    timer2->start();
                }
                else
                {
                    timer->stop();
                    timer2->stop();
                }
                break;
            }
        }
    }
};

int main( int argc, char** argv )
{
    kvs::glut::Application app( argc, argv );
    std::cout << "mark" << std::endl;
    
//    size_t dimx = 10, dimy = 10, dimz = 10;
//    size_t dimx = 25, dimy = 25, dimz = 25;
    size_t dimx = 128, dimy = 128, dimz = 128;
//    size_t dimx = 256, dimy = 256, dimz = 256;
    double minx = -1.0, miny = -1.0, minz = -1.0;
    double maxx =  1.0, maxy =  1.0, maxz =  1.0;
    double dx = ( maxx - minx ) / dimx, dy = ( maxy - miny ) / dimy, dz = ( maxz - minz ) / dimz;
    double x = minx, y = miny, z = minz;
    kvs::AnyValueArray values;
    float* pvalues = static_cast<float*>( values.allocate<float>( dimx * dimy * dimz ) );
    for ( size_t k = 0; k < dimz; k++ )
    {
        z += dz;
        for ( size_t j = 0; j < dimy; j++ )
        {
            y += dy;
            for ( size_t i = 0; i < dimx; i++ )
            {
                x += dx;
                *(pvalues++) = static_cast<float>( f( x, y, z ) );
            }
            x = minx;
        }
        y = miny;
    }

    ::Volume = new kvs::StructuredVolumeObject();
    ::Volume->setGridType( kvs::StructuredVolumeObject::Uniform );
    ::Volume->setResolution( kvs::Vector3ui( dimx, dimy, dimz ) );
    ::Volume->setVeclen( 1 );
    ::Volume->setValues( values );
    ::Volume->updateMinMaxCoords();
    ::Volume->updateMinMaxValues();
    std::cout << *::Volume << std::endl;
    
    x = minx, y = miny, z = minz;
    kvs::AnyValueArray values2;
    float* qvalues = static_cast<float*>( values2.allocate<float>( dimx * dimy * dimz ) );
    for ( size_t m = 0; m < dimz; m++ )
    {
        z += dz;
        for ( size_t n = 0; n < dimy; n++ )
        {
            y += dy;
            for ( size_t q = 0; q < dimx; q++ )
            {
                x += dx;
                *(qvalues++) = static_cast<float>( g( x, y, z ) );
            }
            x = minx;
        }
        y = miny;
    }
    ::Volume2 = new kvs::StructuredVolumeObject();
    ::Volume2->setGridType( kvs::StructuredVolumeObject::Uniform );
    ::Volume2->setResolution( kvs::Vector3ui( dimx, dimy, dimz ) );
    ::Volume2->setVeclen( 1 );
    ::Volume2->setValues( values2 );
    ::Volume2->updateMinMaxCoords();
    ::Volume2->updateMinMaxValues();
    std::cout << std::endl << *::Volume2 << std::endl;
    
    
    kvs::PolygonObject::NormalType n = kvs::PolygonObject::VertexNormal;
    bool d = false;
//    kvs::TransferFunction t( 256 );
    kvs::TransferFunction t;
    t.setRange( 0, 255 );
    kvs::ColorMap cmap;
    cmap.setResolution( 256 );
    cmap.addPoint( 0, kvs::RGBColor( 0, 0 , 255 ) );
    cmap.addPoint( 255, kvs::RGBColor( 0, 0 , 255 ) );
    cmap.create();
    t.setColorMap( cmap );
    
    kvs::TransferFunction t2;
    t2.setRange( 0, 255 );
    kvs::ColorMap cmap2;
    cmap2.setResolution( 256 );
    cmap2.addPoint( 0, kvs::RGBColor( 0, 210, 0 ) );
    cmap2.addPoint( 255, kvs::RGBColor( 0, 210, 0 ) );
    cmap2.create();
    t2.setColorMap( cmap2 );
    
    kvs::PolygonObject* object = new kvs::Isosurface( ::Volume, ::Isolevel, n, d, t );
    kvs::PolygonObject* object2 = new kvs::Isosurface( ::Volume2, ::Isolevel, n, d, t2 );
    object->setName("Isosurface");
    object2->setName("Isosurface2");

    kvs::LineObject* bounds = new kvs::Bounds( ::Volume );
//    delete volume;

    int msec = 100;
    timer = new kvs::glut::Timer( msec );
    timer2 = new kvs::glut::Timer( msec );
    
    UpdateIsolevel update_isolevel( n, d, t, ::Volume, "Isosurface" );
    UpdateIsolevel update_isolevel2( n, d, t2, ::Volume2, "Isosurface2" );
    
    KeyPressEvent keypressevent;
    
    kvs::glut::Screen screen( &app );
    screen.registerObject( object );
    screen.registerObject( object2 );
    screen.registerObject( bounds );
    screen.addTimerEvent( &update_isolevel, timer );
    screen.addTimerEvent( &update_isolevel2, timer2 );
    screen.addKeyPressEvent( &keypressevent );
    screen.setGeometry( 0, 0, 800, 600 );
    screen.show(); 
    
    timer->stop();
    timer2->stop();
    
    return app.run();
}
