#include <kvs/TetrahedralCell>
#include <kvs/UnstructuredVolumeObject>
#include <kvs/UnstructuredVolumeImporter>
#include <kvs/StructuredVolumeObject>
#include <kvs/StructuredVolumeImporter>
#include <kvs/ValueArray>
#include <kvs/Type>
#include <kvs/glew/RayCastingRenderer>
#include <kvs/glut/Screen>
#include <kvs/glut/Application>
#include <kvs/CommandLine>
#include <kvs/File>
#include "BlockLoader.h"
#include "VldImporter.h"

class Argument : public kvs::CommandLine
{
public:
    std::string filename_ori;
    std::string filename_eva;
    
    Argument( int argc, char** argv ) : CommandLine( argc, argv )
    {
        add_help_option();
        addOption( "ori", "input the filename of the original volume", 1, true );
        addOption( "eva", "input the filename of the evaluation volume", 1, true );
    }
    
    void exec()
    {
        if( !this->parse() ) exit( EXIT_FAILURE );
        filename_ori = this->optionValue<std::string>( "ori" );
        filename_eva = this->optionValue<std::string>( "eva" );
    }
};

void CalcBoundingBox( kvs::TetrahedralCell<float>* cell, kvs::Vector3f& min_c, kvs::Vector3f& max_c )
{
    kvs::Vector3f c[4];
    for ( int i = 0; i < 4; ++i )
    {
        c[i] = cell->vertices()[i];
    }

    min_c = c[0];
    max_c = c[0];
    for ( int i = 1; i < 4; ++i )
    {
        for ( int j = 0; j < 3; ++j )
        {
            min_c[j] = kvs::Math::Min( min_c[j], c[i][j] );
            max_c[j] = kvs::Math::Max( max_c[j], c[i][j] );
        }
    }
}

int main( int argc, char** argv )
{
    Argument param( argc, argv );
    param.exec();
    
    kvs::File fileOri( param.filename_ori );
    kvs::File fileEva( param.filename_eva );
    
    kvs::StructuredVolumeObject* ori_volume = NULL;
    kvs::UnstructuredVolumeObject* eva_volume = NULL;
    
    if( fileOri.extension() == "kvsml" )
    {
        ori_volume = new kvs::StructuredVolumeImporter( param.filename_ori );
        std::cout << "use kvsml volume data as original data" << std::endl;
    }
    else if( fileOri.extension() == "vld" )
    {
        ori_volume = new kun::VldImporter( param.filename_ori );
        std::cout << "use vld volume data as original data" << std::endl;
    }
    
    size_t nx = ori_volume->resolution().x();
    size_t ny = ori_volume->resolution().y();
    size_t nz = ori_volume->resolution().z();
    
    if( fileEva.extension() == "kvsml")
    {
        eva_volume = new kvs::UnstructuredVolumeImporter( param.filename_eva );
        std::cout << "use kvsml volume data as evaluation data" << std::endl;
    }
    else if( fileEva.extension() == "zk" )
    {    
        eva_volume = new kun::BlockLoader( param.filename_eva );
        std::cout << "use zk volume data as evaluation data" << std::endl;
    }
    
    float max_value = (float)ori_volume->maxValue();
    float min_value = (float)ori_volume->minValue();
    std::cout << "max valume of the compressed volume:" << max_value << std::endl;
    std::cout << "min valume of the compressed volume:" << min_value << std::endl;
    
    float* ori_values = new float[nx * ny * nz];
    ori_values = (float*)ori_volume->values().pointer();
    
    size_t NCells = eva_volume->ncells();
    kvs::TetrahedralCell<float>* cell = new kvs::TetrahedralCell<float>( eva_volume );
    
    kvs::ValueArray<float> eva_values;
    eva_values.allocate( nx * ny * nz );
    
    for ( size_t count = 0; count < NCells ; count++ )
    {
        cell->bindCell( count );
    
        kvs::Vector3f minf, maxf;
        kvs::Vector3i mini, maxi;
        CalcBoundingBox( cell, minf, maxf );
        
        for ( int a = 0; a < 3; ++a )
        {
            mini[a] = kvs::Math::Floor( minf[a] ); 
            maxi[a] = kvs::Math::Ceil( maxf[a] ); 
        }
        
        if ( mini.x() < 0 || maxi.x() > (float)nx ) std::cout << "x out" << std::endl;
        if ( mini.y() < 0 || maxi.y() > (float)ny ) std::cout << "y out" << std::endl;
        if ( mini.z() < 0 || maxi.z() > (float)nz ) std::cout << "z out" << std::endl;
        
        for ( int k = mini.z(); k <= maxi.z(); k++ )
            for ( int j = mini.y(); j <= maxi.y(); j ++ )
                for ( int i = mini.x(); i <= maxi.x(); i ++ )
                {
                    cell->setGlobalPoint( kvs::Vector3f( i, j, k ) );
                    kvs::Vector3f local = cell->localPoint();
                    if ( !(local.x() < 0 || local.y() < 0 || local.z() < 0 || 1 - local.x() + local.y() + local.z() < 0 ) )
                    {
                        int index = i + j * nx + k * nx * ny;
                        eva_values[ index ] = cell->scalar();
                    }
                }
    }
    
    //Calculate the error
    float error = 0;
    float average_error = 0;    
    for ( unsigned int i = 0; i < nx * ny * nz; i ++ )
    {
        error += fabs(eva_values[i] - ori_values[i]) / ( max_value - min_value ) ;
    }
    average_error = error / ( nx * ny * nz );
    
    std::cout << "whole error: " << error << std::endl;
    std::cout << "The average error is: " << average_error << std::endl;
}
