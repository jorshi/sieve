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
#include "SampleType.h"
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
    
    // Add a sample type keyword for relating samples to a sample type
    void addSampleType(const String& keyword, SampleType::Ptr);
    
private:
    
    using TaggedSamples = std::map<String, Array<Sample*>>;
    using TypeMap = std::map<String, const SampleType::Ptr>;
    
    // Run thread
    void run() override;
    
    void loadSamples();
    
    void exploreDirectory(const File& directory, Array<String>& tags, SampleType::Ptr type = nullptr);
    
    TypeMap sampleTypes_;
    std::queue<SampleFolder::Ptr> sampleFolders_;

    CriticalSection mutex_;
    SampleFolder::Ptr currentSampleFolder_;
    ScopedPointer<WildcardFileFilter> fileFilter_;
    DBConnector db_;
};
