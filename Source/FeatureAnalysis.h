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
    void run(Sample::Ptr sample, AnalysisObject::Ptr analysis, double segStart=0, double segLength=0);
    
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
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(FeatureAnalysis)
    
    
};
