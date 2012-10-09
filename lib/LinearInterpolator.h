//
//  LinearInterpolator.h
//  
//
//  Created by Kun Zhao on 12/10/09.
//  Copyright (c) 2012年 Kyoto University. All rights reserved.
//

#ifndef _LinearInterpolator_h
#define _LinearInterpolator_h
#include <iostream>
#include <map>
#include <vector>

namespace kun 
{

class LinearInterpolator 
{    
public:
    LinearInterpolator();
    void addDataPoint( float x, float &d );
    float interpolate( float x );
    
private:
    std::map<float, float> data;
};
    
} //end of namespace kun

#endif
