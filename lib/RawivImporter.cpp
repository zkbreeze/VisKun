//
//  RawivImporter.cpp
//  
//
//  Created by Kun Zhao on 12/06/06.
//  Copyright (c) 2012 Kun Zhao. All rights reserved.
//

#include "RawivImporter.h"

#include <kvs/File>
#include <kvs/Message>
#include <kvs/Endian>
#include <cstdlib>
#include <cstring>

namespace kun
{

RawivImporter::RawivImporter( void )
{
}

RawivImporter::RawivImporter( const std::string& filename )
{
    this->import( filename );
}
    
RawivImporter::~RawivImporter( void )
{
}

const kvs::Vector3ui& RawivImporter::resolution( void )
{
    return( m_resolution );
}

const kvs::AnyValueArray& RawivImporter::values( void )
{
    return( m_values );
}

const bool RawivImporter::import( const std::string& filename )
{
    FILE* ifs = fopen( filename.c_str(), "rb" );
    if ( !ifs )
    {
        kvsMessageError( "Cannot open file <%s>.", filename.c_str() );
        return( false );
    }

    fseek( ifs, 8 * sizeof(int), SEEK_SET );

    unsigned int dim[3];
    fread( dim, sizeof(unsigned int), 3, ifs );
    kvs::Endian::Swap( dim, 3 );
    m_resolution = kvs::Vector3ui( dim );

    fseek( ifs, 6 * sizeof(float), SEEK_CUR );
    const size_t nnodes = m_resolution.x() * m_resolution.y() * m_resolution.z();

    kvs::File file( filename );
    const size_t byte_size = file.byteSize();
    if ( ( byte_size - 68 ) == nnodes )
    {
        unsigned char* values = new unsigned char[ nnodes ];
        fread( values, sizeof(unsigned char), nnodes, ifs );
        kvs::Endian::Swap( values, nnodes );
        m_values = kvs::AnyValueArray( kvs::ValueArray<unsigned char>( values, nnodes ) );
        delete [] values;
    }
    else if ( ( byte_size - 68 ) == 2 * nnodes )
    {
        unsigned short* values = new unsigned short[ nnodes ];
        fread( values, sizeof(unsigned short), nnodes, ifs );
        kvs::Endian::Swap( values, nnodes );
        m_values = kvs::AnyValueArray( kvs::ValueArray<unsigned short>( values, nnodes ) );
        delete [] values;
    }
    else
    {
        float* values = new float[ nnodes ];
        fread( values, sizeof(float), nnodes, ifs );
        kvs::Endian::Swap( values, nnodes );
        m_values = kvs::AnyValueArray( kvs::ValueArray<float>( values, nnodes ) );
        delete [] values;
    }

    fclose( ifs );
    
    const kvs::StructuredVolumeObject::GridType grid_type = kvs::StructuredVolumeObject::Uniform;
    
    setGridType( grid_type );
    setResolution( m_resolution );
    setVeclen( 1 );
    setValues( m_values );
    updateMinMaxCoords();
    updateMinMaxValues();
    
    return( true );
}

const bool RawivImporter::write( const std::string& filename )
{
    return( true );
    //now it is not needed
}

}
