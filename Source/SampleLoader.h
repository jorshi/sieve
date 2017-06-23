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
    
    void addDirectory(File&);
    
private:
    
    using TaggedSamples = std::map<String, Array<Sample*>>;
    
    // Run thread
    void run() override;
    
    // 
    void loadSamples();
    
    void exploreDirectory(const File& directory, Array<String>& tags);
    
    
    std::queue<File> directories_;
    CriticalSection mutex_;
    ScopedPointer<File> currentlyLoading_;
    ScopedPointer<WildcardFileFilter> fileFilter_;
    DBConnector db_;
};
