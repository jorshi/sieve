/*
  ==============================================================================

    AnalysisObject.cpp
    Created: 7 Aug 2017 6:59:38pm
    Author:  Jordie Shier 

  ==============================================================================
*/

#include "AnalysisObject.h"

AnalysisObject::AnalysisObject(long long id, long long sampleId, double start, double length) : id_(id),
    sampleId_(sampleId), start_(start), length_(length)
{
    
}
