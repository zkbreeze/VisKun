//
//  RawivImporter.h
//  
//
//  Created by Kun Zhao on 12/06/06.
//  Copyright (c) 2012 Kun Zhao. All rights reserved.
//

#ifndef KVS__RAWIV_IMPORTER_H_INCLUDE
#define KVS__RAWIV_IMPORTER_H_INCLUDE

#include <string>
#include <iostream>
#include <vector>
#include <kvs/ImporterBase>
#include <kvs/StructuredVolumeObject>
#include <kvs/Vector3>
#include <kvs/AnyValueArray>

namespace kun
{

    class RawivImporter : public kvs::StructuredVolumeObject
{

private:

    kvs::Vector3ui          m_resolution;
    kvs::AnyValueArray      m_values;

public:

    RawivImporter( void );

    RawivImporter( const std::string& filename );
    
    virtual ~RawivImporter( void );

public:

    const kvs::Vector3ui& resolution( void );

    const kvs::AnyValueArray& values( void );

private:

    const bool import( const std::string& filename );

public:
    
    const bool write( const std::string& filename );

};

}

#endif
