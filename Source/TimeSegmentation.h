//
//  TimeSegmentation.h
//  Sieve
//
//  Created by Jordie Shier  on 2019-10-18.
//  Copyright © 2019 University of Victoria. All rights reserved.
//

#ifndef TimeSegmentation_h
#define TimeSegmentation_h

struct TimeSegmentation
{
    TimeSegmentation() : start(-1.0), length(-1.0), isFullSample(true) {};
    TimeSegmentation(double s, double l) : start(s), length(l), isFullSample(false) {};
    
    const double start;
    const double length;
    const bool isFullSample;
};



#endif /* TimeSegmentation_h */
