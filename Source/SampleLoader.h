/*
  ==============================================================================

    SampleLoader.h
    Created: 12 Jun 2017 7:12:01pm
    Author:  Jordie Shier 

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "dbConnector.h"
#include "Sample.h"
#include "SampleFolder.h"

#include <queue>
#include <mutex>
#include <map>

class SampleLoader : private Thread
{
public:
    
    // Default Constructor
    SampleLoader();
    
    // Default Deconstructor
    ~SampleLoader();
    
    void addSampleFolder(SampleFolder::Ptr);
    
private:
    
    using TaggedSamples = std::map<String, Array<Sample*>>;
    
    // Run thread
    void run() override;
    
    void loadSamples();
    
    void exploreDirectory(const File& directory, Array<String>& tags);
    
    std::queue<SampleFolder::Ptr> sampleFolders_;

    CriticalSection mutex_;
    SampleFolder::Ptr currentSampleFolder_;
    ScopedPointer<WildcardFileFilter> fileFilter_;
    DBConnector db_;
};
