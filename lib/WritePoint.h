//
//  WritePoint.h
//  
//
//  Created by Kun Zhao on 12/11/07.
//  Copyright (c) 2012年 Kyoto University. All rights reserved.
//

#ifndef _WritePoint_h
#define _WritePoint_h

#include <kvs/PointObject>
#include <kvs/KVSMLObjectPoint>
#include <kvs/PointExporter>>

void WritePoint ( kvs::PointObject* object, std::string outname )
{
    kvs::KVSMLObjectPoint* output_volume = new kvs::PointExporter<kvs::KVSMLObjectPoint>( object );
    output_volume->setWritingDataType( kvs::KVSMLObjectPoint::ExternalBinary );
    output_volume->write( outname.c_str() );
    std::cout << "finish writing point volume of " << outname << std::endl;
}


#endif
