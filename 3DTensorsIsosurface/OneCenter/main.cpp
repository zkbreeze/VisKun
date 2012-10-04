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
                          2.777777778,	1.777777778,	0.888888889,
                          1.777777778,	2.777777778,	0.888888889,
                          0.888888889,	0.888888889,	1.444444444),
    //vertex 1
    kvs::Matrix33<double>(
                          3.777777778,	-0.777777778,	-0.888888889,
                          -0.777777778,	3.777777778,	0.888888889,
                          -0.888888889,	0.888888889,	1.444444444),
    //vertex 2
    kvs::Matrix33<double>(
                          3.777777778,	0.777777778,	-0.888888889,
                          0.777777778,	3.777777778,	-0.888888889,
                          -0.888888889,	-0.888888889,	1.444444444),
    //vertex 3
    kvs::Matrix33<double>(
                          3.777777778,	-0.777777778,	0.888888889,
                          -0.777777778,	3.777777778,	-0.888888889,
                          0.888888889,	-0.888888889,	1.444444444),
    //vertex 4
    kvs::Matrix33<double>(
                          3.777777778,	0.777777778,	-0.888888889,
                          0.777777778,	3.777777778,	-0.888888889,
                          -0.888888889,	-0.888888889,	1.444444444),
    //vertex 5
    kvs::Matrix33<double>(
                          3.777777778,	-0.777777778,	0.888888889,
                          -0.777777778,	3.777777778,	-0.888888889,
                          0.888888889,	-0.888888889,	1.444444444),
    //vertex 6
    kvs::Matrix33<double>(
                          3.777777778,	0.777777778,	0.888888889,
                          0.777777778,	3.777777778,	0.888888889,
                          0.888888889,	0.888888889,	1.444444444),
    //vertex 7
    kvs::Matrix33<double>(
                          3.777777778,	-0.777777778,	-0.888888889,
                          -0.777777778,	3.777777778,	0.888888889,
                          -0.888888889,	0.888888889,	1.444444444)    
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

int main( int argc, char** argv )
{
    kvs::glut::Application app( argc, argv );

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

    kvs::StructuredVolumeObject* volume = new kvs::StructuredVolumeObject();
    volume->setGridType( kvs::StructuredVolumeObject::Uniform );
    volume->setResolution( kvs::Vector3ui( dimx, dimy, dimz ) );
    volume->setVeclen( 1 );
    volume->setValues( values );
    volume->updateMinMaxCoords();
    volume->updateMinMaxValues();

    std::cout << *volume << std::endl;
//    cout<<endl;
//    cout<< f(0.0,0.0,0.0) <<endl;
//    cout<< g(0.0,0.0,0.0) <<endl;

    double i = 1;
    kvs::PolygonObject::NormalType n = kvs::PolygonObject::VertexNormal;
    bool d = false;
    kvs::TransferFunction t( 256 );
    kvs::PolygonObject* object = new kvs::Isosurface( volume, i, n, d, t );

    kvs::LineObject* bounds = new kvs::Bounds( volume );
    delete volume;

    kvs::glut::Screen screen( &app );
    screen.registerObject( object );
    screen.registerObject( bounds );
    screen.show();

    return app.run();
}
