/*
  ==============================================================================

    SampleManager.h
    Created: 12 Jun 2017 7:11:12pm
    Author:  Jordie Shier 

  ==============================================================================
*/

#pragma once

#include "Mapping.h"

#include "../JuceLibraryCode/JuceHeader.h"
#include "SampleLoader.h"
#include "SampleAnalysis.h"
#include "Sample.h"
#include "SampleType.h"
#include "SampleFolder.h"
#include "SampleReduced.h"
#include "DirectoryChooser.h"
#include "dbConnector.h"
#include "FileLoader.h"
#include "DimensionReduction.h"

#include <map>

class SampleManager
{
public:
    
    // Default Constructor
    SampleManager();
    
    // Default Deconstructor
    ~SampleManager();
    
    // Prompt user to input directory for new samples
    void loadNewSamples();

    // Load samples onto grid based on sample type selected
    void updateGrid(const int& sampleType);
    
    // Get a pointer to one of the samples int he sample buffers
    Sample::Ptr getSample(int num) const;
    
    // Load thumbnail caches
    void updateThumbnails();
    
    // Gets the audio format reader for a given sample
    AudioFormatReader* getReaderForSample(Sample& sample);
    
    // Read sample folders from database
    void readSampleFolders();
    
    // Get a reference to all the sample folders
    ReferenceCountedArray<SampleFolder>& getSampleFolders() { return sampleFolders_; };
    
    // Update grid to children of the new root
    void zoom(Sample::Ptr newRoot);
    
    // Zoom all the way back out
    void zoomOutFull();
    
    // Clear out grid and samples
    void clear();
    
private:
    
    // Initialize db connection
    DBConnector db_;
    FileLoader loader_;
    
    // Thumbnails
    ScopedPointer<AudioThumbnailCache> thumbnailCache_;
    
    // Dummy root sample to hold the sample distribution tree
    Sample::Ptr root_;
    
    // Current samples and queued samples. Queued sampels are collected as they are loaded
    // from the database on a select before being moved into the current sample array
    ReferenceCountedArray<Sample> currentSamples_;
    ReferenceCountedArray<Sample> queuedSamples_;
    std::vector<SampleReduced::Ptr> samplesReduced_;
    
    // List of loaded sample folders
    ReferenceCountedArray<SampleFolder> sampleFolders_;
    
    ReferenceCountedArray<SampleType> sampleTypesBuffer_;
    std::map<String, SampleType::Ptr> sampleTypes_;
    
    ScopedPointer<SampleLoader> sampleLoader_;
    ScopedPointer<SampleAnalysis> analysis_;
    ScopedPointer<DimensionReduction> dimensionReduction_;
    DirectoryChooser directoryChooser_;
    
    Mapping sampleMapping_;



    //==============================================================================
    
    // Creates the default sample types required for operation - currently just snare and kick
    void setupTypes();
    
    // Recursively distribute loaded samples into sample pads
    void distributeSamples(std::vector<SampleReduced::Ptr>& samples, Sample::Ptr root);
    
    // Static callback for a select sample query
    static int selectSampleCallback(void *param, int argc, char **argv, char **azCol)
    {
        SampleManager* manager = reinterpret_cast<SampleManager*>(param);
        if (argc == 9)
        {
            Sample::Ptr newSample = new Sample(atoi(argv[0]),
                                               String(CharPointer_UTF8(argv[1])),
                                               String(CharPointer_UTF8(argv[2])),
                                               atof(argv[3]),
                                               atof(argv[4]),
                                               atoi(argv[5]),
                                               atoi(argv[6]),
                                               atoi(argv[7]),
                                               atoi(argv[8]));

            manager->queuedSamples_.add(newSample);
        }
        return 0;
    }
    
    // Static Callback for a select sample folder query
    static int selectSampleFolderCallback(void *param, int argc, char **argv, char **azCol)
    {
        SampleManager* manager = reinterpret_cast<SampleManager*>(param);
        if (argc > 3)
        {
            SampleFolder::Ptr newFolder = new SampleFolder(atoi(argv[0]), String(argv[1]));
            newFolder->setStatus(atoi(argv[2]));
            newFolder->setNumSamples(atoi(argv[3]));
            manager->sampleFolders_.add(newFolder);
        }
        return 0;
    }
    
    // Static Callback for a select sample type query
    static int selectSampleTypeCallback(void *param, int argc, char **argv, char **azCol)
    {
        SampleManager* manager = reinterpret_cast<SampleManager*>(param);
        if (argc > 1)
        {
            SampleType::Ptr newType = new SampleType(atoi(argv[0]), String(argv[1]));
            manager->sampleTypesBuffer_.add(newType);
        }
        return 0;
    }
    
    // Static Callback for a select samples reduced query
    static int selectSamplesReducedCallback(void *param, int argc, char **argv, char **azCol)
    {
        SampleManager* manager = reinterpret_cast<SampleManager*>(param);
        
        // This also quered for the associated sample object
        if (argc == 14)
        {
            SampleReduced::Ptr newSampleReduced = new SampleReduced(atoi(argv[0]), atoi(argv[1]), atoi(argv[2]), atof(argv[3]), atof(argv[4]));
            
            Sample::Ptr newSample = new Sample(atoi(argv[5]),
                                               String(CharPointer_UTF8(argv[6])),
                                               String(CharPointer_UTF8(argv[7])),
                                               atof(argv[8]),
                                               atof(argv[9]),
                                               atoi(argv[10]),
                                               atoi(argv[11]),
                                               atoi(argv[12]),
                                               atoi(argv[13]));
            
            newSampleReduced->setSamplePtr(newSample);
            manager->samplesReduced_.push_back(newSampleReduced);
            
        }
        return 0;
    }
    
    
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SampleManager)
};
