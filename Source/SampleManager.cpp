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
    root_ = new Sample();
    
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

SampleManager::~SampleManager()
{

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
        
        double x;
        double y;
        double dist;
        double angle;
        double maxDist = 0;
        
        // Calculate magniutde and angle for each sample position which will be used for colours
        std::vector<std::pair<double, double>> magAngle;
        for (auto sample = samplesReduced_.begin(); sample != samplesReduced_.end(); ++sample)
        {
            x = (*sample)->getX();
            y = (*sample)->getY();
            
            dist = sqrt(x*x + y*y);
            angle = atan2(y, x) / (M_PI * 2);
            
            magAngle.push_back(std::pair<double, double>(dist, angle));
            
            if (dist > maxDist)
            {
                maxDist = dist;
            }
        }
        
        // Add colours to each sample
        for (int i = 0; i < samplesReduced_.size(); i++)
        {
            samplesReduced_[i]->getSamplePtr()->setColour(Colour(magAngle[i].second, magAngle[i].first / maxDist, 1.0f, 1.0f));
        }
        
        
        // Clear out old samples
        currentSamples_.clear();
        root_->getChildren().clear();
        
        // Distribute samples across sample pads
        distributeSamples(samplesReduced_, root_);
        currentSamples_ = root_->getChildren();
        updateThumbnails();
    }
}


void SampleManager::distributeSamples(std::vector<SampleReduced::Ptr>& samples, Sample::Ptr root)
{
    // Recieve an array of samples -- connected to a rooot
    
    // If there are more than 64 samples then apply K-means clustering
    // and make a recursive call for each cluster to this function
    if (samples.size() > 64)
    {
        // Temporary vector of doubles to represent 2D positions
        std::vector<std::vector<double>> reducedMap;
        
        // Put samples into a vector matrix
        for (int i = 0; i < samples.size(); i++)
        {
            reducedMap.push_back({samples.at(i)->getX(), samples.at(i)->getY()});
        }
        
        std::vector<std::pair<std::vector<int>, int>> clusters = sampleMapping_.cluster(reducedMap);
        jassert(clusters.size() == 64);
        
        std::vector<SampleReduced::Ptr> topLevelSamples(64);
        for (int i = 0; i < clusters.size(); i++)
        {
            // Top level samples
            SampleReduced::Ptr representativeSample = samples.at(clusters.at(i).second);
            topLevelSamples.at(i) = representativeSample;
            
            if (clusters.at(i).first.size() > 1)
            {
                // Copy the sample representing the subset of samples
                SampleReduced::Ptr newSubsetSampleReduced = new SampleReduced(*representativeSample);
                Sample::Ptr newSubsetSample = new Sample(*newSubsetSampleReduced->getSamplePtr());
                newSubsetSampleReduced->setSamplePtr(newSubsetSample);
                representativeSample->getSamplePtr()->setSubsetSamples((int)clusters.at(i).first.size());
                
                // Replace the sample with the copy for use in the subset layer
                samples.at(clusters.at(i).second) = newSubsetSampleReduced;
                
                std::vector<SampleReduced::Ptr> subsetSamples;
                for (int j = 0; j < clusters.at(i).first.size(); j++)
                {
                    // Set the parent sample and push into the subset for distribution
                    SampleReduced::Ptr subsetSample = samples.at(clusters.at(i).first.at(j));
                    subsetSample->getSamplePtr()->setParent(representativeSample->getSamplePtr());
                    subsetSamples.push_back(samples.at(clusters.at(i).first.at(j)));
                }
                
                // Make a recursive call for this subset now
                distributeSamples(subsetSamples, representativeSample->getSamplePtr());
            }
        }
        
        distributeSamples(topLevelSamples, root);
        
    }
    
    // Otherwise map each sample to a pad on the grid and return
    else
    {
        // Initialize the child samples for the root pad
        ReferenceCountedArray<Sample>& childSamples = root->getChildren();
        childSamples.clear();
        for (int i = 0; i < 64; i++)
        {
            childSamples.insert(i, nullptr);
        }
        
        // Temporary vector of doubles to represent 2D positions
        std::vector<std::vector<double>> reducedMap;
        
        // Put samples into a vector matrix
        for (int i = 0; i < samples.size(); i++)
        {
            reducedMap.push_back({samples.at(i)->getX(), samples.at(i)->getY()});
        }
        
        std::vector<int> assignments = sampleMapping_.mapToGrid(reducedMap);
        for (int i = 0; i < assignments.size(); i++)
        {
            Sample::Ptr samplePlacement = samples.at(i)->getSamplePtr();
            samplePlacement->setDisplay(String(samples.at(i)->getX(), 2) +
                                        "," + String(samples.at(i)->getY(), 2));
            //samplePlacement->setSubsetSamples((int)assignments.size());
            childSamples.set(assignments[i], samplePlacement);
        }
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
            //std::cout << r << " " << g << " " << b << "\n";
            
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

void SampleManager::zoom(Sample::Ptr newRoot)
{
    currentSamples_ = newRoot->getChildren();
    updateThumbnails();
}

void SampleManager::zoomOutFull()
{
    currentSamples_ = root_->getChildren();
    updateThumbnails();
}

void SampleManager::clear()
{
    currentSamples_.clear();
    root_->getChildren().clear();
    updateThumbnails();
}
