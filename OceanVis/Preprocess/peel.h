//
//  peel.h
//  
//
//  Created by Kun Zhao on 12/09/18.
//  Copyright (c) 2012 Kyoto University. All rights reserved.
//

#include <iostream>
#include <kvs/StructuredVolumeObject>
#include <kvs/StructuredVolumeImporter>
#include <kvs/KVSMLObjectStructuredVolume>
#include <kvs/StructuredVolumeExporter>

kvs::StructuredVolumeObject* peel( kvs::StructuredVolumeObject* object, size_t slice )
{
    size_t nx = object->resolution().x();
    size_t ny = object->resolution().y();
    size_t nz = object->resolution().z();
    std::cout << nx << std::endl;
    std::cout << ny << std::endl;
    std::cout << nz << std::endl;
    
    float* pvalues = (float*)object->values().pointer();
    
    size_t nx_new = nx - 2 * slice;
    size_t ny_new = ny - 2 * slice;
    size_t nz_new = nz;
    
    kvs::AnyValueArray values;
    float* buf = static_cast<float*>( values.allocate<float>( nx_new * ny_new * nz_new) );
    for ( size_t k = 0; k < nz_new; k ++ )
        for ( size_t j = 0; j < ny_new; j ++ )
            for ( size_t i = 0; i < nx_new; i ++ )
            {
                size_t index = i + j * nx_new + k * nx_new * ny_new;
                size_t index_ori = ( i + slice ) + ( j + slice ) * nx + k * nx * ny;
                buf[index] = pvalues[index_ori];
            }
    
    kvs::Vector3ui resolution( nx_new, ny_new, nz_new );
    kvs::VolumeObjectBase::GridType grid_type = kvs::VolumeObjectBase::Uniform;
    kvs::StructuredVolumeObject* volume = new kvs::StructuredVolumeObject();
    volume->setGridType( grid_type);
    volume->setVeclen( 1 );
    volume->setResolution( resolution );
    volume->setValues( values );
    
    volume->updateMinMaxCoords();
    volume->updateMinMaxValues();    
    return volume;    
}
