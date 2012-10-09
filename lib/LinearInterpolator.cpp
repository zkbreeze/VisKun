//
//  LinearInterpolator.cpp
//  
//
//  Created by Kun Zhao on 12/09/11.
//  Copyright (c) 2012 Kyoto University. All rights reserved.
//

#include "LinearInterpolator.h"

namespace kun {

    LinearInterpolator::LinearInterpolator() {}
    
    void LinearInterpolator::addDataPoint(float x, float &d) {
        // just add it to the map
        data[x] = d;
    }
    
    float LinearInterpolator::interpolate(float x ) {
        // loop through all the keys in the map
        // to find one that is greater than our intended value
        std::map< float, float >::iterator it = data.begin();
        bool found = false;
        while(it != data.end() && !found) {
            if(it->first >= x) {
                found = true;
                break;
            }
            
            // advance the iterator
            it++;
        }
        
        // check to see if we're outside the data range
        if(it == data.begin()) {
            return data.begin()->second;
        }
        else if(it == data.end()) {
            // move the point back one, as end() points past the list
            it--;
            return it->second;
        }
        // check to see if we landed on a given point
        else if(it->first == x) {
            return it->second;
        }
        
        // nope, we're in the range somewhere
        // collect some values
        float xb = it->first;
        float yb = it->second;
        it--;
        float xa = it->first;
        float ya = it->second;
        
        // and calculate the result!
        // formula from Wikipedia
        return (ya + (yb - ya) * (x - xa) / (xb - xa));
    }
}; //end of namespace kun
