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
            std::cout << "can PCA!\n";
        }
        
        else if (std::all_of(sampleFolders_.begin(), sampleFolders_.end(), [](SampleFolder::Ptr f){ return f->getStatus() == 4; }))
        {
            // Everything has already been run through dimension reduction
            signalThreadShouldExit();
        }
            
        wait(1000);
    }
    
}
