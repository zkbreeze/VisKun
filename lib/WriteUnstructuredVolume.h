//
//  WriteUnstructuredVolume.h
//  
//
//  Created by Kun Zhao on 12/11/07.
//  Copyright (c) 2012年 Kyoto University. All rights reserved.
//

#ifndef _WriteUnstructuredVolume_h
#define _WriteUnstructuredVolume_h

#include <kvs/UnstructuredVolumeObject>
#include <kvs/KVSMLObjectUnstructuredVolume>
#include <kvs/UnstructuredVolumeExporter>>

void WriteUnstructuredVolume ( kvs::UnstructuredVolumeObject* object, std::string outname )
{
    kvs::KVSMLObjectUnstructuredVolume* output_volume = new kvs::UnstructuredVolumeExporter<kvs::KVSMLObjectUnstructuredVolume>( object );
    output_volume->setWritingDataType( kvs::KVSMLObjectUnstructuredVolume::ExternalBinary );
    output_volume->write( outname.c_str() );
    std::cout << "finish writing unstrucutured volume of " << outname << std::endl;
}


#endif
