//
//  WriteStructuredVolume.h
//  
//
//  Created by Kun Zhao on 12/11/07.
//  Copyright (c) 2012年 Kyoto University. All rights reserved.
//

#ifndef _WriteStructuredVolume_h
#define _WriteStructuredVolume_h

#include <kvs/StructuredVolumeObject>
#include <kvs/KVSMLObjectStructuredVolume>
#include <kvs/StructuredVolumeExporter>>

void WriteStructuredVolume ( kvs::StructuredVolumeObject* object, std::string outname )
{
    kvs::KVSMLObjectStructuredVolume* output_volume = new kvs::StructuredVolumeExporter<kvs::KVSMLObjectStructuredVolume>( object );
    output_volume->setWritingDataType( kvs::KVSMLObjectStructuredVolume::ExternalBinary );
    output_volume->write( outname.c_str() );
    std::cout << "finish writing Strucutured volume of " << outname << std::endl;
}

#endif
