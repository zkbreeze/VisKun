//
//  main.cpp
//  
//
//  Created by Kun Zhao on 12/05/29.
//  Copyright (c) 2012 Kyoto University. All rights reserved.
//

#include <kvs/UnstructuredVolumeImporter>
#include <kvs/UnstructuredVolumeObject>
#include <kvs/Timer>
#include <cstdio>
#include <kvs/CommandLine>
#include <kvs/BitArray>
#include <kvs/Timer>
#include <kvs/File>
#include <kvs/StructuredVolumeObject>
#include <kvs/StructuredVolumeImporter>
#include "BlockLoader.h"
#include "JetImporter.h"
#include "CreateString.h"

class Argument : public kvs::CommandLine
{
public:
    std::string basename;
    float tolerance;
    size_t nsteps;
    
    Argument( int argc, char** argv ) : CommandLine( argc, argv )
    {
        add_help_option();
        addOption( "b", "basename of the time-varying data", 1, true );
        addOption( "t", "tolerance of the CT", 1, false );
        addOption( "n", "n time steps", 1, true );
    }
    
    void exec()
    {
        tolerance = 0.001;
        if( !this->parse() ) exit( EXIT_FAILURE );
        if( this->hasOption( "b" ) ) basename = this->optionValue<std::string>( "b" );
        if( this->hasOption( "t" ) ) tolerance = this->optionValue<float>( "t" );
        if( this->hasOption( "n" ) ) nsteps = this->optionValue<size_t>( "n" );
    }
};

int main( int argc, char** argv )
{
    Argument param( argc, argv );
    param.exec();
    
    const float* value1 = NULL;
    const float* value2 = NULL;
    float range;
    unsigned int NumberOfVertices;
    size_t nsteps = param.nsteps;
    
    kvs::StructuredVolumeObject** object = new kvs::StructuredVolumeObject*[16];
    for( size_t i = 0; i < nsteps; i++ )
    {
        std::string filename = CreateString( param.basename, i, "kvsml" );
        object[i] = new kvs::StructuredVolumeImporter( filename );
        std::cout << "finish loading " << filename << std::endl;
    }
    
    int nnodes = 0;
    for( size_t i = 0; i < (nsteps - 1); i++ )
    {
        kvs::StructuredVolumeObject* object1 = object[i];
        kvs::StructuredVolumeObject* object2 = object[i + 1];
        value1 = (float*)object1->values().pointer();
        value2 = (float*)object2->values().pointer();
        range = (float)( object1->maxValue() - object1->minValue() );
        NumberOfVertices = object1->nnodes();
        float toleranceError = range * param.tolerance;    
        
        kvs::Timer time;
        time.start();
        std::vector<float> com;
        bool* VerticesTable = new bool[NumberOfVertices];
        
        //Make vertices table
        float error = 0.0;
        size_t countVT = 0;
        kvs::Timer time1;
        time1.start();
        for ( size_t i = 0; i < NumberOfVertices; i++ ) 
        {
            if ( fabs(value1[i] - value2[i]) <= toleranceError )
            { 
                VerticesTable[i] = (bool)1; 
                countVT ++;
                error += fabs(value1[i] - value2[i]) / range;
            }
            else
            {
                VerticesTable[i] = (bool)0;
                com.push_back( value2[i] );
            }
        }
        time1.stop();
        std::cout << "time for VT: " << time1.msec() << "ms" << std::endl;
        std::cout << "number of vertices: " << NumberOfVertices << std::endl;
        std::cout << "number of vertices in compression data: " <<com.size() << std::endl;
        nnodes += com.size();
    }
    std::cout << "average number: " << (float)nnodes/(nsteps - 1) << std::endl;
}