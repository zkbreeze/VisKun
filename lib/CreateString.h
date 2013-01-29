//
//  CreateString.h
//  
//
//  Created by Kun Zhao on 13/01/29.
//  Copyright (c) 2013年 Kyoto University. All rights reserved.
//

#ifndef _CreateString_h
#define _CreateString_h

#include <iostream>

std::string CreateString( std::string filename, int step, std::string extension )
{
    char buf[10];
    sprintf( buf, "%03d", step );
    return( filename + std::string( buf ) + "." + extension );
}

#endif
