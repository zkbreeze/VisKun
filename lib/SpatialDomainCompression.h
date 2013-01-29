//
//  SpatialDomainCompression.h
//  
//
//  Created by Kun Zhao on 13/01/25.
//  Copyright (c) 2013年 Kyoto University. All rights reserved.
//

#ifndef _SpatialDomainCompression_h
#define _SpatialDomainCompression_h
#include <kvs/StructuredVolumeObject>
#include <kvs/UnstructuredVolumeObject>
#include "ConvertToSpline.h"
#include "SDC.h"

namespace kun
{

    class SpatialDomainCompression : public kun::SDC
    {
        
    public:
        
        SpatialDomainCompression( kvs::StructuredVolumeObject* volume, size_t block_size );
        
        kun::SDC* compression( kvs::StructuredVolumeObject* object );
        
        kvs::UnstructuredVolumeObject* decompression( kun::SDC* sdc_object );
    };
    
} // end of namespace kun



#endif
