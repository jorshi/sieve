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
    currentSampleFolder_ = nullptr;
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
        if (sampleFolders_.size() < 1 && currentSampleFolder_ == nullptr)
        {
            signalThreadShouldExit();
        }
        
        // If no directory is being explored currently and the directory queue is not
        // empty then mark the next directory for loading
        else if (sampleFolders_.size() > 0 && currentSampleFolder_ == nullptr)
        {
            currentSampleFolder_ = sampleFolders_.front();
            sampleFolders_.pop();
        }
        mutex_.exit();
        
        // Load samples from the directoy contained in the currently loading pointer
        if (currentSampleFolder_ != nullptr)
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
    tags.add(currentSampleFolder_->getFile().getFileName());
    
    // Recursively explore the given folder
    exploreDirectory(currentSampleFolder_->getFile(), tags);

    // Finished exploring the directory
    currentSampleFolder_->setStatus(2);
    currentSampleFolder_->update(db_);
    currentSampleFolder_ = nullptr;
}


void SampleLoader::exploreDirectory(const File& directory, Array<String>& tags, SampleType::Ptr type)
{
    Array<String> newTags = tags;
    DirectoryIterator iter(directory, false, "*", File::findFilesAndDirectories);
    bool isDirectory;
    
    while (iter.next(&isDirectory, nullptr, nullptr, nullptr, nullptr, nullptr))
    {
        if (isDirectory)
        {
            String dirName = iter.getFile().getFileName();
            newTags.add(dirName);
            
            TypeMap::iterator it = sampleTypes_.find(dirName);
            if (it != sampleTypes_.end())
            {
                type = it->second;
            }
            
            exploreDirectory(iter.getFile(), newTags, type);
        }
        else if (fileFilter_->isFileSuitable(iter.getFile()))
        {
            long long typeId = 0;
            if (type != nullptr)
            {
                typeId = type->getId();
            }
            Sample newSample(0, iter.getFile().getFileName(), iter.getFile().getFullPathName(),
                             0, 0, false, currentSampleFolder_->getId(), typeId);
            
            newSample.save(db_);
            currentSampleFolder_->incrementSamples();
        }
    }
}

void SampleLoader::addSampleFolder(SampleFolder::Ptr folder)
{
    mutex_.enter();
    sampleFolders_.push(folder);
    mutex_.exit();
    
    if (!isThreadRunning())
    {
        startThread();
    }
}

void SampleLoader::addSampleType(const String& keyword, const SampleType::Ptr sampleType)
{
    sampleTypes_.insert(std::pair<String, const SampleType::Ptr>(keyword, sampleType));
}
