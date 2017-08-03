/*
  ==============================================================================

    SampleAnalysis.cpp
    Created: 5 Jul 2017 12:07:58am
    Author:  Jordie Shier 

  ==============================================================================
*/

#include "SampleAnalysis.h"


SampleAnalysis::SampleAnalysis() : Thread("Sample Analysis Thread")
{
    currentSampleFolder_ = nullptr;
}


SampleAnalysis::~SampleAnalysis()
{
    stopThread(4000);
}


void SampleAnalysis::run()
{
    while(!threadShouldExit())
    {
        // Enter critical section
        mutex_.enter();
        
        // If there are no sample folders left to analyze then exit thread
        if (sampleFolders_.size() < 1 && currentSampleFolder_ == nullptr)
        {
            signalThreadShouldExit();
        }
        
        // Get next sample folder to run analysis on
        else if (sampleFolders_.size() > 0 && currentSampleFolder_ == nullptr)
        {
            currentSampleFolder_ = sampleFolders_.front();
            sampleFolders_.pop();
        }
        
        mutex_.exit();
        
        if (currentSampleFolder_ != nullptr && currentSampleFolder_->getStatus() == 2)
        {
            std::cout << "Running analysis on " << currentSampleFolder_->getFile().getFileName() << "\n";
        }        
        
        wait(1000);
    }
}

void SampleAnalysis::addSampleFolder(SampleFolder::Ptr folder)
{
    mutex_.enter();
    sampleFolders_.push(folder);
    mutex_.exit();
    
    if (!isThreadRunning())
    {
        startThread();
    }
}
