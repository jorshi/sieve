/*
  ==============================================================================

    SampleAnalysis.h
    Created: 5 Jul 2017 12:07:58am
    Author:  Jordie Shier 

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include <essentia/algorithmfactory.h>
#include <essentia/streaming/algorithms/poolstorage.h>
#include <essentia/scheduler/network.h>


class SampleAnalysis
{
public:
    
    // Default Constructor
    SampleAnalysis();
    
    // Default Decontstructor
    ~SampleAnalysis() {};
    
private:
    
    
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SampleAnalysis)
};

