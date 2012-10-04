//
//  RawImporter.cpp
//  
//
//  Created by Kun Zhao on 12/06/06.
//  Copyright (c) 2012 Kun Zhao. All rights reserved.
//

#include "RawImporter.h"

#include <kvs/File>
#include <kvs/Message>
#include <kvs/Endian>
#include <cstdlib>
#include <cstring>

namespace kvs
{

RawImporter::RawImporter( void )
{
}

RawImporter::RawImporter( 
                         const std::string& filename, 
                         size_t ResolutionX,
                         size_t ResolutionY,
                         size_t ResolutionZ
                         )   
{
    m_resolution = kvs::Vector3ui( ResolutionX, ResolutionY, ResolutionZ );
    this->import( filename );
}
    
RawImporter::RawImporter( const std::string& filename )
{
    m_resolution = kvs::Vector3ui( 256, 256, 256 );
    this->import( filename );
}
    
RawImporter::~RawImporter( void )
{
}

const kvs::Vector3ui& RawImporter::resolution( void )
{
    return( m_resolution );
}

const kvs::AnyValueArray& RawImporter::values( void )
{
    return( m_values );
}

const bool RawImporter::import( const std::string& filename )
{
    FILE *rawfile = fopen( filename.c_str(), "rb" );
    size_t nx = m_resolution.x();
    size_t ny = m_resolution.y();
    size_t nz = m_resolution.z();
    size_t length = nx * ny * nz;
    
    unsigned char* buf = static_cast<unsigned char*>( m_values.allocate<unsigned char>( length ) );
    fread( buf, sizeof( unsigned char ), length, rawfile );
    fclose(rawfile) ;
    
    const kvs::StructuredVolumeObject::GridType grid_type = kvs::StructuredVolumeObject::Uniform;
    
    setGridType( grid_type );
    setResolution( m_resolution );
    setVeclen( 1 );
    setValues( m_values );
    updateMinMaxCoords();
    updateMinMaxValues();
    
    return( true );
}

const bool RawImporter::write( const std::string& filename )
{
    return( true );
    //now it is not needed
}

}
