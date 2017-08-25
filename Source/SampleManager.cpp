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
    analysis_ = new SampleAnalysis(db_);
    
    thumbnailCache_ = new AudioThumbnailCache(64);
    
    // Load in sample folders from database
    readSampleFolders();
    setupTypes();
    
    dimensionReduction_ = new DimensionReduction(db_, sampleFolders_);

    // Check current folders for any unfinished loading or analysis
    for (auto folder = sampleFolders_.begin(); folder != sampleFolders_.end(); ++folder)
    {
        switch ((*folder)->getStatus())
        {
            case 0:
                (*folder)->updateStatus(1, db_);
            case 1:
                sampleLoader_->addSampleFolder(*folder);
            case 2:
                analysis_->addSampleFolder(*folder);
                break;
            case 3:
                break;
        }
    }
    
    
}


void SampleManager::setupTypes()
{
    Array<String> defaultTypes = {"kick", "snare"};
    SampleType::Ptr newType;
    
    // Load default sample types. If they don't exist in the database yet then create
    // them and save into database
    for (auto type = defaultTypes.begin(); type != defaultTypes.end(); ++type)
    {
        sampleTypesBuffer_.clear();
        String sql = "SELECT * FROM `sample_type` WHERE `name` = '" + *type + "';";
        if (db_.runCommand(sql, selectSampleTypeCallback, this))
        {
            if (sampleTypesBuffer_.size() > 0)
            {
                sampleTypes_.insert(std::pair<String, SampleType::Ptr>(*type, sampleTypesBuffer_.getUnchecked(0)));
                sampleLoader_->addSampleType(*type, sampleTypesBuffer_.getUnchecked(0));
            }
            else
            {
                newType = new SampleType(0, *type);
                newType->save(db_);
                sampleTypes_.insert(std::pair<String, SampleType::Ptr>(*type, newType));
                sampleLoader_->addSampleType(*type, newType);
            }
        }
    }
    sampleTypesBuffer_.clear();
}


void SampleManager::loadNewSamples()
{
    File directory;
    if (directoryChooser_.getDirectory(directory))
    {
        // Add sample folder to DB and update the current sample folder list
        SampleFolder::Ptr newFolder = new SampleFolder;
        newFolder->setPath(directory);
        if (newFolder->save(db_)) {
            newFolder->updateStatus(1, db_);
            sampleFolders_.add(newFolder);
            sampleLoader_->addSampleFolder(newFolder);
            analysis_->addSampleFolder(newFolder);
            dimensionReduction_->runDimensionReduction();
        }
    }
}


void SampleManager::updateGrid(const int &sampleType)
{
    String sql = "SELECT r.*, s.* FROM `samples_reduced` r " \
        "JOIN samples s ON s.id = r.sample_id " \
        "WHERE s.sample_type = " + String(sampleType) + ";";
    
    samplesReduced_.clear();
    if (db_.runCommand(sql, selectSamplesReducedCallback, this))
    {
        Sample::Ptr root = new Sample();
        ReferenceCountedArray<Sample>& samples = root->getChildren();
        
        for (int i = 0; i < 64; i++)
        {
            samples.insert(i, nullptr);
        }

        
        if (samplesReduced_.size() > 64)
        {
            // Do clustering
        }
        else
        {
            Mapping sampleMapping;
            
            std::vector<std::vector<double>> reducedMap;
            
            // Put samples into a vector matrix
            for (int i = 0; i < samplesReduced_.size(); i++)
            {
                reducedMap.push_back({samplesReduced_.at(i)->getX(), samplesReduced_.at(i)->getY()});
            }
            
            std::vector<int> assignments = sampleMapping.mapToGrid(reducedMap);
            for (int i = 0; i < assignments.size(); i++)
            {
                Sample::Ptr samplePlacement = samplesReduced_.at(i)->getSamplePtr();
                samplePlacement->setDisplay(String(samplesReduced_.at(i)->getX(), 2) +
                                            "," + String(samplesReduced_.at(i)->getY(), 2));
                
                samples.set(assignments[i], samplePlacement);
            }
        }
        
        currentSamples_ = samples;
        updateThumbnails();
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
        if (*sample == nullptr) continue;
        AudioThumbnail* newThumbnail = new AudioThumbnail(512, loader_.getFormatManager(), *thumbnailCache_);
        newThumbnail->setSource(new FileInputSource((*sample)->getFile()));
        (*sample)->setThumbnail(newThumbnail);
    }
}


// This is just here for testing
void SampleManager::updateRainbowColours()
{
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
            
            sample = currentSamples_.getUnchecked(count);
            sample->setColour(Colour::fromRGB(r,g,b));
            std::cout << r << " " << g << " " << b << "\n";
            
            count++;
        }
    }
}


AudioFormatReader* SampleManager::getReaderForSample(Sample& sample)
{
    return loader_.getAudioReader(sample.getFile());
}


void SampleManager::readSampleFolders()
{
    sampleFolders_.clear();
    String sql = "SELECT * FROM `sample_folders` LIMIT 50;";
    db_.runCommand(sql, selectSampleFolderCallback, this);
}
