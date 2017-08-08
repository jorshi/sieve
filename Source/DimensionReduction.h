/*
  ==============================================================================

    DimensionReduction.h
    Created: 8 Aug 2017 12:49:23pm
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
#include "AnalysisObject.h"

class DimensionReduction : private Thread
{
public:
    
    DimensionReduction(const DBConnector& db, const ReferenceCountedArray<SampleFolder>& f);
    
    ~DimensionReduction();
    
    void runDimensionReduction();
    
private:
    
    const DBConnector& db_;
    const ReferenceCountedArray<SampleFolder>& sampleFolders_;
    
    // Struct for keeping track of the different sample types and the specific segmentation to use
    struct SampleClassPCA
    {
        SampleClassPCA(int t, double s, double l) : sampleType(t), segStart(t), segLength(l) {};
        
        int sampleType;
        double segStart;
        double segLength;
    };
    
    OwnedArray<SampleClassPCA> sampleClasses_;
    
    void run() override;
    
    void pca();
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DimensionReduction)
    
};
