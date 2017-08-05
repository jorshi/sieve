/*
  ==============================================================================

    SampleAnalysis.h
    Created: 5 Jul 2017 12:07:58am
    Author:  Jordie Shier 

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include <queue>

#include "Sample.h"
#include "SampleFolder.h"
#include "dbConnector.h"
#include "FeatureAnalysis.h"


class SampleAnalysis : private Thread
{
public:
    
    // Default Constructor
    SampleAnalysis(const DBConnector& db);
    
    // Default Decontstructor
    ~SampleAnalysis();
    
    // Add a sample folder to queue
    void addSampleFolder(SampleFolder::Ptr folder);
    
private:
    
    CriticalSection mutex_;
    SampleFolder::Ptr currentSampleFolder_;
    
    // Reference to a database connection
    const DBConnector& db_;
    
    // Queue of sample folders that need to be analyzed
    std::queue<SampleFolder::Ptr> sampleFolders_;
    
    ReferenceCountedArray<Sample> analysisSamples_;
    
    // Feature analysis object
    ScopedPointer<FeatureAnalysis> analysis_;
    
    // Static callback for a select sample query
    static int selectSampleCallback(void *param, int argc, char **argv, char **azCol)
    {
        SampleAnalysis* analysisObj = reinterpret_cast<SampleAnalysis*>(param);
        if (argc == 8)
        {
            Sample::Ptr newSample = new Sample(atoi(argv[0]),
                                               String(argv[1]),
                                               String(argv[2]),
                                               atof(argv[3]),
                                               atof(argv[4]),
                                               atoi(argv[5]),
                                               atoi(argv[6]),
                                               atoi(argv[7]));
            
            analysisObj->analysisSamples_.add(newSample);
        }
        return 0;
    }
    
    // Run thread
    void run() override;
    
    void runAnalysisBatch();
    
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SampleAnalysis)
};

