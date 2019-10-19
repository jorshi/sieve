/*
  ==============================================================================

    SampleAnalysis.cpp
    Created: 5 Jul 2017 12:07:58am
    Author:  Jordie Shier 

  ==============================================================================
*/

#include "SampleAnalysis.h"


SampleAnalysis::SampleAnalysis(const DBConnector& db) : Thread("Sample Analysis Thread"), db_(db)
{
    currentSampleFolder_ = nullptr;
    analysis_ = nullptr;
    
    // Time segmentations to run
    segmentations_.add(new TimeSegmentation(.20, .025));
    segmentations_.add(new TimeSegmentation(.20, .1));
    segmentations_.add(new TimeSegmentation(.20, .25));
    segmentations_.add(new TimeSegmentation(.20, .5));
    segmentations_.add(new TimeSegmentation(.50, .025));
    segmentations_.add(new TimeSegmentation(.50, .1));
    segmentations_.add(new TimeSegmentation(.50, .25));
    segmentations_.add(new TimeSegmentation(.50, .5));
    segmentations_.add(new TimeSegmentation(.90, .025));
    segmentations_.add(new TimeSegmentation(.90, .1));
    segmentations_.add(new TimeSegmentation(.90, .25));
    segmentations_.add(new TimeSegmentation(.90, .5));
}


SampleAnalysis::~SampleAnalysis()
{
    stopThread(8000);
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
            analysis_.reset();
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
            if (analysis_ == nullptr)
            {
                try {
                    analysis_ = new FeatureAnalysis(db_);
                } catch (std::exception& e) {
                    //std::cout << e.what() << "\n";
                }
            }
            runAnalysisBatch();
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


void SampleAnalysis::runAnalysisBatch()
{
    String sql = "SELECT * FROM `samples` WHERE sample_folder = " + String(currentSampleFolder_->getId()) + \
        " AND analyzed = 0 AND exclude = 0;";
    
    analysisSamples_.clear();
    db_.runCommand(sql, selectSampleCallback, this);

    int remainingSamples = analysisSamples_.size();
    int totalSamples = currentSampleFolder_->getNumSamples();
    currentSampleFolder_->setPercentAnalyzed(100 * (double(totalSamples)-remainingSamples) / totalSamples);
    
    if (remainingSamples == 0)
    {
        currentSampleFolder_->updateStatus(3, db_);
        currentSampleFolder_ = nullptr;
        return;
    }
    
    for (auto sample = analysisSamples_.begin(); sample != analysisSamples_.end(); ++sample)
    {
        if (threadShouldExit()) return;
        
        try
        {
            analysis_->run(*sample, segmentations_);
            (*sample)->setAnalyzed(1);
            (*sample)->updateSave(db_);
            remainingSamples--;
            currentSampleFolder_->setPercentAnalyzed(100 * (double(totalSamples)-remainingSamples) / totalSamples);
        }
        catch (std::exception& e)
        {
            DBG(e.what());
            analysis_ = nullptr;
            return;
        }
    }
}
