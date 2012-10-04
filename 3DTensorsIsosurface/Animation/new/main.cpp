//
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
#include <float.h>

namespace { kvs::StructuredVolumeObject* Volume = NULL; }
namespace { double Isolevel = 1e-12; }
//namespace { double Isolevel = 10; }
kvs::glut::Timer* timer;

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
//        std::cout << "                       " << "\r" << std::flush;
//        std::cout << "isolevel: " << ::Isolevel << "\r" << std::flush;

//        if ( 100.0 <= ::Isolevel && ::Isolevel <= 255.0 ) ::Isolevel -= 10.0;
//        else if ( 10.0 < ::Isolevel && ::Isolevel <= 100.0 ) ::Isolevel -= 5.0;
//        else if ( 1.0 < ::Isolevel && ::Isolevel <= 10.0 ) ::Isolevel -= 1.0;
//        else if ( 1.0e-10 < ::Isolevel && ::Isolevel <= 1.0 ) ::Isolevel /= 2.0;
        
//        if ( 1.0e-15 < ::Isolevel && ::Isolevel <= 1.0 ) ::Isolevel /= 5.0;
//        else ::Isolevel = 1.0;

        kvs::PolygonObject* object = new kvs::Isosurface( m_volume, ::Isolevel, m_norm, m_dup, m_tfunc );
        object->setName( m_objectname );

//        // calculate the Min(da)
//        size_t number_of_vertice = object->nnormals();
//        std::cout << number_of_vertice << std::endl;
//        float min_value = FLT_MAX;
//        for ( size_t i = 0; i < number_of_vertice ; i++)
//        {
//            double x_d3 = ( (double)object->coord( i ).x() / 127.0 ) * 2.0 - 1.0;
//            double y_d3 = ( (double)object->coord( i ).y() / 127.0 ) * 2.0 - 1.0;
//            double z_d3 = ( (double)object->coord( i ).z() / 127.0 ) * 2.0 - 1.0;
////            std::cout << x_d3 << std::endl;
////            std::cout << y_d3 << std::endl;
////            std::cout << z_d3 << std::endl;
//            float da = static_cast<float>( g( x_d3, y_d3, z_d3 ) );
//            if( da < min_value )
//                min_value = da;
//        }
//        std::cout << min_value << std::endl;
        
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
    
    // calculate the proposed division number
    double error = 1e-12;
    for ( size_t dn = 3; dn < 600; dn += 2)
    {
        double x_dn = 1.0 / dn;
        double y_dn = 1.0 / dn;
        double z_dn = 1.0 / dn;
        
        if ( f ( x_dn, y_dn, z_dn ) <= error ) 
            std::cout << dn << std::endl;
        
    }

    ::Volume = new kvs::StructuredVolumeObject();
    ::Volume->setGridType( kvs::StructuredVolumeObject::Uniform );
    ::Volume->setResolution( kvs::Vector3ui( dimx, dimy, dimz ) );
    ::Volume->setVeclen( 1 );
    ::Volume->setValues( values );
    ::Volume->updateMinMaxCoords();
    ::Volume->updateMinMaxValues();
    std::cout << *::Volume << std::endl;
    
//    kvs::LineObject* object = new kvs::ExtractEdges( ::Volume );
    
    
    kvs::PolygonObject::NormalType n = kvs::PolygonObject::VertexNormal;
    bool d = false;
    kvs::TransferFunction t( 256 );
    kvs::PolygonObject* object = new kvs::Isosurface( ::Volume, ::Isolevel, n, d, t );
    object->setName("Isosurface");

    kvs::LineObject* bounds = new kvs::Bounds( ::Volume );
//    delete volume;

    int msec = 100;
    timer = new kvs::glut::Timer( msec );

    UpdateIsolevel update_isolevel( n, d, t, ::Volume, "Isosurface" );
    KeyPressEvent keypressevent;

    kvs::glut::Screen screen( &app );
    screen.registerObject( object );
    screen.registerObject( bounds );
    screen.addTimerEvent( &update_isolevel, timer );
    screen.addKeyPressEvent( &keypressevent );
    screen.show(); 

    return app.run();
}
