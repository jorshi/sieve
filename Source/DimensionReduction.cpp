/*
  ==============================================================================

    DimensionReduction.cpp
    Created: 8 Aug 2017 12:49:23pm
    Author:  Jordie Shier 

  ==============================================================================
*/

#include "DimensionReduction.h"

DimensionReduction::DimensionReduction(const DBConnector& db, const ReferenceCountedArray<SampleFolder>& f) :
    Thread("Dimension Reduction Thread"), db_(db), sampleFolders_(f)
{
    if (sampleFolders_.size())
    {
        startThread();
    }
    
    sampleClasses_.add(new SampleClassPCA(1, 0.2, 0.1));    // Kick Drum PCA Segmentations
    sampleClasses_.add(new SampleClassPCA(2, 0.5, 0.25));   // Snare Drum PCA Segmentations
}


DimensionReduction::~DimensionReduction()
{
    stopThread(4000);
}


void DimensionReduction::runDimensionReduction()
{
    startThread();
}


void DimensionReduction::run()
{
    while(!threadShouldExit())
    {
        if (std::all_of(sampleFolders_.begin(), sampleFolders_.end(), [](SampleFolder::Ptr f){ return f->getStatus() == 3; }))
        {
            pca();
        }
        
        else if (std::all_of(sampleFolders_.begin(), sampleFolders_.end(), [](SampleFolder::Ptr f){ return f->getStatus() == 4; }))
        {
            // Everything has already been run through dimension reduction
            signalThreadShouldExit();
        }
            
        wait(1000);
    }
}

void DimensionReduction::pca()
{
    for (auto sampleClass = sampleClasses_.begin(); sampleClass != sampleClasses_.end(); ++sampleClass)
    {
        if (!threadShouldExit())
        {
            String sql = "SELECT * FROM `samples` WHERE " \
                "sample_type = " + String((*sampleClass)->sampleType) + \
                " AND exclude = 0;";
            
            std::cout << sql << "\n";
            
        }
    }
}
