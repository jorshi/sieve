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
    TimeSegmentation(double s, double l) : start(s), length(l) {};
    double start;
    double length;
};

#endif /* TimeSegmentation_h */
