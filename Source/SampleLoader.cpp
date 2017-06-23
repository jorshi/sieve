/*
  ==============================================================================

    SampleLoader.cpp
    Created: 12 Jun 2017 7:12:01pm
    Author:  Jordie Shier 

  ==============================================================================
*/

#include "SampleLoader.h"


SampleLoader::SampleLoader() : Thread("Sample Loading Thread")
{
    currentlyLoading_ = nullptr;
    fileFilter_ = new WildcardFileFilter("*.wav", "", "audio file filter");
}


SampleLoader::~SampleLoader()
{
    stopThread(4000);
}


void SampleLoader::run()
{
    // Directory exploration is performed on a background thread
    while(!threadShouldExit())
    {
        // The directories variable can be accessed from this thread as well
        // as the parent, make sure that they don't get accessed at the same time
        mutex_.enter();
        
        // If the directory queue is empty and no directories are being loaded
        // then this thread can shutdown
        if (directories_.size() < 1 && currentlyLoading_ == nullptr)
        {
            signalThreadShouldExit();
        }
        
        // If no directory is being explored currently and the directory queue is not
        // empty then mark the next directory for loading
        else if (directories_.size() > 0 && currentlyLoading_ == nullptr)
        {
            currentlyLoading_ = new File(directories_.front());
            directories_.pop();
        }
        mutex_.exit();
        
        // Load samples from the directoy contained in the currently loading pointer
        if (currentlyLoading_ != nullptr)
        {
            loadSamples();
        }

        wait(1000);
    }
}


void SampleLoader::loadSamples()
{
    // Arrays for holding the folder names as the directories are explored
    Array<String> tags;
    Array<Sample> samples;
    TaggedSamples sampleTags;

    // Save the top level directory name
    tags.add(currentlyLoading_->getFileName());
    
    // Recursively explore the given folder
    exploreDirectory(*currentlyLoading_, tags);

    // Finished exploring the directory
    currentlyLoading_.release();
}


void SampleLoader::exploreDirectory(const File& directory, Array<String>& tags)
{
    Array<String> newTags = tags;
    DirectoryIterator iter(directory, false, "*", File::findFilesAndDirectories);
    bool isDirectory;
    
    while (iter.next(&isDirectory, nullptr, nullptr, nullptr, nullptr, nullptr))
    {
        if (isDirectory)
        {
            newTags.add(iter.getFile().getFileName());
            exploreDirectory(iter.getFile(), newTags);
        }
        else if (fileFilter_->isFileSuitable(iter.getFile()))
        {
            Sample newSample(0, iter.getFile().getFileName(), iter.getFile().getFullPathName(), 0, 0);
            newSample.save(db_);
            //newSample.saveTagsForSample(db_, newTags);
        }
    }
}


void SampleLoader::addDirectory(File& directory)
{
    mutex_.enter();
    directories_.push(directory);
    mutex_.exit();
    
    if (!isThreadRunning())
    {
        startThread();
    }
}
