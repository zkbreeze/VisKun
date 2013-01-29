//
//  VldImporter.h
//  
//
//  Created by Kun Zhao on 13/01/23.
//  Copyright (c) 2013 Kun Zhao. All rights reserved.
//

#ifndef KVS__VLD_IMPORTER_H_INCLUDE
#define KVS__VLD_IMPORTER_H_INCLUDE

#include <string>
#include <iostream>
#include <vector>
#include <kvs/ImporterBase>
#include <kvs/StructuredVolumeObject>
#include <kvs/Vector3>
#include <kvs/AnyValueArray>


//test

namespace kun
{

    class VldImporter : public kvs::StructuredVolumeObject
{

private:

    kvs::Vector3ui          m_resolution;
    kvs::AnyValueArray      m_values;

public:

    VldImporter( void );

    VldImporter( const std::string& filename );
    
    virtual ~VldImporter( void );

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
