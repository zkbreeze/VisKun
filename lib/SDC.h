//
//  SDC.h
//  
//
//  Created by Kun Zhao on 13/01/25.
//  Copyright (c) 2013年 Kyoto University. All rights reserved.
//

#ifndef _SDC_h
#define _SDC_h

#include <iostream>
#include <kvs/StructuredVolumeObject>
#include <kvs/Vector3>
#include "ConvertToSpline.h"

namespace kun
{
    
class SDC
{
    size_t m_block_size;
    kvs::Vector3ui m_resolution;
    float* m_values;
    
    size_t calculate_vertices_number( void );
    
public:
    
    SDC( void );
    
    SDC( std::string filename );
    
    SDC( size_t block_size, kvs::Vector3ui resolution, float* values );
    
    void setBlockSize( size_t block_size );
    
    void setResolution( kvs::Vector3ui resolution );
    
    void setValues( float* values );
    
    size_t blockSize( void );
    
    kvs::Vector3ui resolution( void );
    
    float* values( void ); 
    
    void read( std::string filename );
    
    void write( std::string filename );
};

} // end of namespace of kun
#endif
