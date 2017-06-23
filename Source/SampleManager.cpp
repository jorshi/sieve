/*
  ==============================================================================

    SampleManager.cpp
    Created: 12 Jun 2017 7:11:12pm
    Author:  Jordie Shier 

  ==============================================================================
*/

#include "SampleManager.h"

SampleManager::SampleManager()
{
    sampleLoader_ = new SampleLoader;
    thumbnailCache_ = new AudioThumbnailCache(64);
}

void SampleManager::loadNewSamples()
{
    File directory;
    if (directoryChooser_.getDirectory(directory))
    {
        sampleLoader_->addDirectory(directory);
    }
}


void SampleManager::updateGridRandom()
{
    queuedSamples_.clear();
    String sql = "SELECT * FROM `samples` LIMIT 64;";
    if(db_.runCommand(sql, selectSampleCallback, this))
    {
        currentSamples_.swapWith(queuedSamples_);
        queuedSamples_.clear();
        updateThumbnails();
        updateRainbowColours();
    }
}


Sample::Ptr SampleManager::getSample(int num) const
{
    if (num < currentSamples_.size())
    {
        return currentSamples_.getUnchecked(num);
    }
    
    return nullptr;
}


void SampleManager::updateThumbnails()
{
    for (auto sample = currentSamples_.begin(); sample < currentSamples_.end(); ++sample)
    {
        AudioThumbnail* newThumbnail = new AudioThumbnail(512, loader_.getFormatManager(), *thumbnailCache_);
        newThumbnail->setSource(new FileInputSource((*sample)->getFile()));
        (*sample)->setThumbnail(newThumbnail);
    }
}

void SampleManager::updateRainbowColours()
{
                //newButton = new PadButton("Pad " + std::to_string(count), Colour::fromRGB((i*10) + 100, (j*10) + 100, 150 - (i+j)*2), over, down);
    int numSamples = currentSamples_.size();
    int count = 0;
    Sample::Ptr sample;
    
    for (int i = 0;  i < 8; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            if (count >= numSamples)
            {
                return;
            }
            
            // Set rainbow
            int r = i*15 + 100;
            int g = j*10 + 100;
            int b = 150 - (i*j)*2;
            
            //int r = i*4 + 85;
            //int g = j*2 + 170;
            //int b = 160 - (i*j)*1;
    
            //int r = i*1 + 113;
            //int g = j*1 + 168;
            //int b = 150 - (i*j)*0.5;
            
            sample = currentSamples_.getUnchecked(count);
            sample->setColour(Colour::fromRGB(r,g,b));
            std::cout << r << " " << g << " " << b << "\n";
            
            count++;
        }
    }
}
