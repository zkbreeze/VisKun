//
//  WritePolygon.h
//  
//
//  Created by Kun Zhao on 12/11/07.
//  Copyright (c) 2012年 Kyoto University. All rights reserved.
//

#ifndef _WritePolygon_h
#define _WritePolygon_h

#include <kvs/PolygonObject>
#include <kvs/KVSMLObjectPolygon>
#include <kvs/PolygonExporter>>

void WritePolygon ( kvs::PolygonObject* object, std::string outname )
{
    kvs::KVSMLObjectPolygon* output_volume = new kvs::PolygonExporter<kvs::KVSMLObjectPolygon>( object );
    output_volume->setWritingDataType( kvs::KVSMLObjectPolygon::ExternalBinary );
    output_volume->write( outname.c_str() );
    std::cout << "finish writing polygon volume of " << outname << std::endl;
}

#endif
