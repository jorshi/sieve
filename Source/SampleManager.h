/*
  ==============================================================================

    SampleManager.h
    Created: 12 Jun 2017 7:11:12pm
    Author:  Jordie Shier 

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "SampleLoader.h"
#include "Sample.h"
#include "DirectoryChooser.h"
#include "dbConnector.h"
#include "FileLoader.h"

class SampleManager
{
public:
    
    // Default Constructor
    SampleManager();
    
    // Default Deconstructor
    ~SampleManager() {};
    
    // Prompt user to input directory for new samples
    void loadNewSamples();
    
    // Loads a random selection of samples into the current sample buffer
    void updateGridRandom();
    
    // Get a pointer to one of the samples int he sample buffers
    Sample::Ptr getSample(int num) const;
    
    // Load thumbnail caches
    void updateThumbnails();
    
    // Rainbow colours for testing!
    void updateRainbowColours();
    
    // Gets the audio format reader for a given sample
    AudioFormatReader* getReaderForSample(Sample& sample);
    
private:
    
    // Static callback for a select sample query
    static int selectSampleCallback(void *param, int argc, char **argv, char **azCol)
    {
        SampleManager* manager = reinterpret_cast<SampleManager*>(param);
        if (argc == 5)
        {
            Sample::Ptr newSample = new Sample();
            newSample->setId(atoi(argv[0]));
            newSample->setName(String(argv[1]));
            newSample->setPath(String(argv[2]));
            manager->queuedSamples_.add(newSample);
        }

        return 0;
    }
    
    // Current samples and queued samples. Queued sampels are collected as they are loaded
    // from the database on a select before being moved into the current sample array
    ReferenceCountedArray<Sample> currentSamples_;
    ReferenceCountedArray<Sample> queuedSamples_;
    
    // Thumbnails
    ScopedPointer<AudioThumbnailCache> thumbnailCache_;
    
    ScopedPointer<SampleLoader> sampleLoader_;
    DirectoryChooser directoryChooser_;
    DBConnector db_;
    FileLoader loader_;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SampleManager)
};
