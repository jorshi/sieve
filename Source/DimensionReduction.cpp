/*
  ==============================================================================

    DimensionReduction.cpp
    Created: 8 Aug 2017 12:49:23pm
    Author:  Jordie Shier 

  ==============================================================================
*/

#include "DimensionReduction.h"

#include "tnt/tnt2vector.h"
#include "tnt/jama_lu.h"
#include "tnt/jama_eig.h"

using namespace essentia;
using namespace standard;

DimensionReduction::DimensionReduction(const DBConnector& db, const ReferenceCountedArray<SampleFolder>& f) :
    Thread("Dimension Reduction Thread"), db_(db), sampleFolders_(f)
{
    if (sampleFolders_.size())
    {
        startThread();
    }
    
    sampleClasses_.add(new SampleClassPCA(1, 0.2, 0.1));    // Kick Drum PCA Segmentations
    sampleClasses_.add(new SampleClassPCA(2, 0.5, 0.25));   // Snare Drum PCA Segmentations
    
    essentia::init();
    AlgorithmFactory& factory = AlgorithmFactory::instance();
    
    pca_ = factory.create("PCA",
                          "namespaceIn", "feature",
                          "namespaceOut", "pca");
    
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
            pca();
        }
        
        else if (std::all_of(sampleFolders_.begin(), sampleFolders_.end(), [](SampleFolder::Ptr f){ return f->getStatus() == 4; }))
        {
            // Everything has already been run through dimension reduction
            signalThreadShouldExit();
        }
            
        wait(1000);
    }
}

void DimensionReduction::pca()
{
    for (auto sampleClass = sampleClasses_.begin(); sampleClass != sampleClasses_.end(); ++sampleClass)
    {
        if (!threadShouldExit())
        {
            // Get all samples for a sample type
            String sql = "SELECT a.* FROM analysis a JOIN samples s ON a.sample_id = s.id " \
                "WHERE s.sample_type = " + String((*sampleClass)->sampleType) + \
                " AND s.exclude = 0;";

            analysisMatrix_.clear();
            sampleOrder_.clear();
            db_.runCommand(sql, selectAnalysisPoolCallback, this);
            
            // Check to see if we need to exit the thread
            if (threadShouldExit()) return;
            
            // Definitely doing more work here than needs to be done -- converting from one orientation to another
            // and then back -- TODO fix this up!
            std::vector<std::vector<Real>> rotated(analysisMatrix_[0].size());
            for (int i = 0; i < analysisMatrix_.size(); i++)
            {
                for (int j = 0; j < analysisMatrix_[0].size(); j++)
                {
                    rotated[j].push_back(analysisMatrix_[i][j]);
                }
            }
            
            // Standardize data
            for (auto feature = rotated.begin(); feature != rotated.end(); ++feature)
            {
                Real sum = std::accumulate(feature->begin(), feature->end(), 0.0);
                Real mean = sum / feature->size();
                
                std::vector<Real> diff(feature->size());
                std::transform(feature->begin(), feature->end(), diff.begin(), std::bind2nd(std::minus<Real>(), mean));
                
                Real sqSum = std::inner_product(feature->begin(), feature->end(), diff.begin(), 0.0);
                Real stdDev = std::sqrt(sqSum / feature->size());
                
                std::transform(feature->begin(), feature->end(), feature->begin(), [mean, stdDev](Real v){ return (v - mean) / stdDev; });
            }
            
            for (int i = 0; i < analysisMatrix_.size(); i++)
            {
                for (int j = 0; j < analysisMatrix_[0].size(); j++)
                {
                    analysisMatrix_[i][j] = rotated[j][i];
                }
            }
            
            // Check to see if we need to exit the thread
            if (threadShouldExit()) return;
            
            // Pools for Essentia PCA
            Pool pcaIn;
            Pool pcaOut;
            
            // Store analysis matrix in a pool
            pcaIn.merge("feature", analysisMatrix_, "replace");
            
            // Calculate PCA
            pca_->reset();
            pca_->input("poolIn").set(pcaIn);
            pca_->output("poolOut").set(pcaOut);
            pca_->compute();
            
            std::map<std::string, std::vector<std::vector<Real>>> output =  pcaOut.getVectorRealPool();
            
        }
    }
}
