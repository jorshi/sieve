/*
  ==============================================================================

    FeatureAnalysis.h
    Created: 4 Aug 2017 9:53:11am
    Author:  Jordie Shier 

  ==============================================================================
*/

#pragma once

#include <essentia/algorithmfactory.h>
#include <essentia/essentiamath.h>
#include <essentia/pool.h>

#include "Sample.h"
#include "AnalysisObject.h"
#include "TimeSegmentation.h"
#include "dbConnector.h"

using namespace essentia;
using namespace essentia::standard;

class FeatureAnalysis
{
public:
    
    // Constructor
    FeatureAnalysis(const DBConnector& db);
    
    // Default Deconstructor
    ~FeatureAnalysis();
    
    // Function call operator to run analysis
    void run();
    void run(Sample::Ptr sample, OwnedArray<TimeSegmentation>& segmentations);
    
private:
    
    const DBConnector& db_;
    
    // PARAMS
    int sampleRate_;
    int frameSize_;
    int hopSize_;
    
    // Algorithms
    Algorithm* audio_;
    Algorithm* audioEq_;
    Algorithm* spectral_;
    Algorithm* spectralEq_;
    Algorithm* aggr_;
    Algorithm* frameCutter_;
    Algorithm* startStopSilence_;
    Algorithm* envelope_;
    Algorithm* lat_;
    Algorithm* centroid_;
    Algorithm* trimmer_;
    Algorithm* rms_;
    
    // Private Functions
    void resetFactoryAlgorithms();
    void loadAudio(Sample::Ptr sample, std::vector<Real>& buffer, Algorithm* loader);
    void computeSampleStartAndStopTime(Sample::Ptr sample, std::vector<Real>& buffer);
    void trimAudioBuffer(std::vector<Real>& buffer, Real startTrim, Real endTrim);
    void computeTemporalFeatures(std::vector<Real>& buffer, Pool& pool);
    void computeSpectralEqLoudFeatures(std::vector<Real>& buffer, Pool& pool);
    void computeSpectralFeatures(std::vector<Real>& buffer, Pool& pool);
    void computeSegmentPool(std::vector<Real>& buffer, TimeSegmentation& segmentation, Pool& inputPool, Pool& outputPool);
    
    template<typename T>
    void segmentPool(const std::map<std::string, std::vector<T>>& inputFeatures, Pool& outputPool, long startFrame, long endFrame);
    
    void savePool(Sample::Ptr sample, Pool& pool, TimeSegmentation& segmentation);
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(FeatureAnalysis)
};
