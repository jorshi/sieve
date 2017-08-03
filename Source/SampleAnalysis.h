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
#include <queue>

#include "SampleFolder.h"


class SampleAnalysis : private Thread
{
public:
    
    // Default Constructor
    SampleAnalysis();
    
    // Default Decontstructor
    ~SampleAnalysis();
    
    // Add a sample folder to queue
    void addSampleFolder(SampleFolder::Ptr folder);
    
private:
    
    // Run thread
    void run() override;
    
    // Queue of sample folders that need to be analyzed
    std::queue<SampleFolder::Ptr> sampleFolders_;
    
    CriticalSection mutex_;
    SampleFolder::Ptr currentSampleFolder_;
    
    
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SampleAnalysis)
};

