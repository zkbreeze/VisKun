//
//  VldImporter.cpp
//  
//
//  Created by Kun Zhao on 12/06/06.
//  Copyright (c) 2012 Kun Zhao. All rights reserved.
//

#include "VldImporter.h"

#include <iostream>
#include <fstream>
#include <cstring>
#include <vector>
#include <cstdlib>

namespace kun
{

VldImporter::VldImporter( void )
{
}

    
VldImporter::VldImporter( const std::string& filename )
{
    this->import( filename );
}
    
VldImporter::~VldImporter( void )
{
}

const kvs::Vector3ui& VldImporter::resolution( void )
{
    return( m_resolution );
}

const kvs::AnyValueArray& VldImporter::values( void )
{
    return( m_values );
}

const bool VldImporter::import( const std::string& filename )
{
    std::ifstream fileVld;
    fileVld.open( filename.c_str() );
    char buf[256];

    if (!fileVld) 
    {
        std::cerr << "ERROR : Cannot open file \"" << filename << "\"." << std::endl;
        return false;
    }
    
    // Read VLD header
    fileVld >> buf;
    if( strncmp( buf, "%FORMAT:VLD", strlen("%FORMAT:VLD") ) != 0 ) 
    {
        std::cerr << "ERROR - Vld::ReadVld(const char[], RegularGridData*)\n";
        std::cerr << "    \"" << filename
        << "\"  is not a VLD-fomat file." << std::endl;
        return false;
    }
    
    fileVld >> m_resolution.x() >> m_resolution.y() >> m_resolution.z();
    size_t length = m_resolution.x() * m_resolution.y() * m_resolution.z();
    
    float* pvalues = static_cast<float*>( m_values.allocate<float>( length ) );
    for( size_t i = 0; i < length; i++ )
        fileVld >> pvalues[i];
    fileVld.close();
    
    const kvs::StructuredVolumeObject::GridType grid_type = kvs::StructuredVolumeObject::Uniform;
    
    setGridType( grid_type );
    setResolution( m_resolution );
    setVeclen( 1 );
    setValues( m_values );
    updateMinMaxCoords();
    updateMinMaxValues();
    
    return( true );
}

const bool VldImporter::write( const std::string& filename )
{
    return( true );
    //now it is not needed
}

}
