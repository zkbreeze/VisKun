//
//  SDC.cpp
//  
//
//  Created by Kun Zhao on 13/01/25.
//  Copyright (c) 2013 Kyoto University. All rights reserved.
//

#include "SDC.h"

namespace kun 
{
    SDC::SDC( void ) {}
    
    SDC::SDC( std::string filename )
    {
        this->read( filename );
    }

    SDC::SDC( size_t block_size, kvs::Vector3ui resolution, float* values )
    {
        m_block_size = block_size;
        m_resolution = resolution;
        m_values = values;
    }
    
    void SDC::setBlockSize( size_t block_size )
    {
        m_block_size = block_size;
    }
    
    void SDC::setResolution( kvs::Vector3ui resolution )
    {
        m_resolution = resolution;
    }
    
    void SDC::setValues( float* values )
    {
        m_values = values;
    }
    
    size_t SDC::blockSize( void )
    {
        return m_block_size;
    }
    
    kvs::Vector3ui SDC::resolution( void )
    {
        return m_resolution;
    }
    
    float* SDC::values( void )
    {
        return m_values;
    }
    
    size_t SDC::calculate_vertices_number( void )
    {
        const kvs::Vector3ui ncells = m_resolution - kvs::Vector3ui( 1, 1, 1 );
        
        const int remainder_x = ncells.x() % m_block_size;
        const int remainder_y = ncells.y() % m_block_size;
        const int remainder_z = ncells.z() % m_block_size;
        
        const size_t block_x = ncells.x() / m_block_size + (bool)( remainder_x );
        const size_t block_y = ncells.y() / m_block_size + (bool)( remainder_y );       
        const size_t block_z = ncells.z() / m_block_size + (bool)( remainder_z );        
        const size_t ncubes = block_x * block_y * block_z;
        const size_t nvertices = ( block_x + 1 ) * ( block_y + 1 ) * ( block_z + 1);
        const size_t nnodes = nvertices + block_x * block_y * ( block_z + 1 ) + block_y * block_z * ( block_x + 1 ) + block_z * block_x * ( block_y + 1 ) + ncubes;
        return nnodes;
    }
    
    void SDC::read( std::string filename )
    {
        FILE *rawfile = fopen( filename.c_str(), "rb" );
        fseek( rawfile, 0, SEEK_END );
        size_t end = ftell( rawfile );
        size_t length = end / sizeof(float);
        fseek( rawfile, 0 , SEEK_SET );    
        float buf[4];
        fread( buf, sizeof( float ), 4, rawfile );
        m_values = new float[ length - 4 ];
        fread( &m_values[0], sizeof( float ), length - 4, rawfile );
        fclose(rawfile);
        
        m_block_size = (unsigned int)buf[0];
        m_resolution.x() = (unsigned int)buf[1]; 
        m_resolution.y() = (unsigned int)buf[2]; 
        m_resolution.z() = (unsigned int)buf[3];
    }
    
    void SDC::write( std::string filename )
    {
        size_t nnodes = calculate_vertices_number();
        std::cout << nnodes << std::endl;
        unsigned int length = nnodes + 4;
        float* buf = new float[length];
        buf[0] = (float)m_block_size;
        buf[1] = (float)m_resolution.x();
        buf[2] = (float)m_resolution.y();
        buf[3] = (float)m_resolution.z();
        for( size_t i = 0; i < nnodes; i++ ) buf[i + 4] = m_values[i];
        
        FILE* outputFile = fopen( filename.c_str(), "wb" );
        fwrite( buf, sizeof(float), length, outputFile );
        std::cout << "finish writting the sdc file: " << filename << std::endl;
    }
}
