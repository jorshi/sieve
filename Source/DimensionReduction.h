/*
  ==============================================================================

    DimensionReduction.h
    Created: 8 Aug 2017 12:49:23pm
    Author:  Jordie Shier 

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include <queue>

#include <essentia/algorithmfactory.h>
#include <essentia/essentiamath.h>
#include <essentia/pool.h>

#include "Sample.h"
#include "SampleFolder.h"
#include "SampleType.h"
#include "dbConnector.h"
#include "FeatureAnalysis.h"
#include "AnalysisObject.h"
#include "SampleReduced.h"
#include "tsne.h"


static const bool USE_MIXED_TIME_SEGMENTATIONS = true;


class DimensionReduction : private Thread
{
public:
    
    DimensionReduction(const DBConnector& db, const ReferenceCountedArray<SampleFolder>& f);
    
    ~DimensionReduction();
    
    void runDimensionReduction();
    
private:
    
    const DBConnector& db_;
    const ReferenceCountedArray<SampleFolder>& sampleFolders_;
    ReferenceCountedArray<Sample> samples_;
    ReferenceCountedArray<AnalysisObject> analyses_;
    
    // List of sample types from DB
    ReferenceCountedArray<SampleType> sampleTypes_;
    
    // Essentia Pool object for storing matrix of analyses prior to PCA
    std::vector<std::vector<Real>> analysisMatrix_;
    std::vector<int> sampleOrder_;
    Pool pcaIn_;
    
    // PCA Algorithm
    Algorithm* pca_;
    
    // TSNE Algorithm
    ScopedPointer<TSNE> tsne_;
    
    // Struct for keeping track of the different sample types and the specific segmentation to use
    struct SampleTypeAndSegmentation
    {
        SampleTypeAndSegmentation(int t, double s, double l) : sampleType(t), segStart(s), segLength(l) {};
        
        int sampleType;
        double segStart;
        double segLength;
    };
    
    struct SampleSegmentation
    {
        SampleSegmentation() : numSamples(0), segStart(0), segLength(0) {};
        SampleSegmentation(long n, double s, double l) : numSamples(n), segStart(s), segLength(l) {};
        
        friend bool operator <(const SampleSegmentation& l, const SampleSegmentation& r)
        {
            return std::tie(l.segStart, l.segLength) < std::tie(r.segStart, r.segLength);
        }
        
        friend bool operator ==(const SampleSegmentation& l, const SampleSegmentation& r)
        {
            return std::tie(l.segStart, l.segLength) == std::tie(r.segStart, r.segLength);
        }
        
        long numSamples;
        double segStart;
        double segLength;
    };
    
    
    OwnedArray<SampleTypeAndSegmentation> sampleClasses_;
    std::vector<std::unique_ptr<SampleSegmentation>> sampleSegmentations_;
    
    // Private Membmer Functions
    void run() override;
    void reduceDimensionality();
    void loadSampleTypes();
    void loadDataWithSegmentation(SampleType::Ptr, SampleSegmentation& segmentation);
    void loadDataWithMixedSegmentations(SampleType::Ptr type);
    void loadSegmentationsForSampleType(SampleType::Ptr type);
    void loadAnalysisSamples(const SampleType::Ptr type, const SampleSegmentation& segmentation);
    void computeFeatureVariance(std::vector<Real>& outputVariance);
    void constructMixedTimeSegmentationAnalysisMatrix(const SampleType::Ptr type, const std::vector<SampleSegmentation>& featureSegmentations);
    void copyFeaturesFromAnalysisMatrix(const std::vector<int>& featureList, std::vector<std::vector<Real>>& outputMatrix);
    
    void preprocess();
    void pca();
    void tsne();
    void setupTsne();
    
    
    
    //===========================================================
    // Static DB Callbacks
    
    // Static callback for a select sample type query
    static int selectSampleTypeCallback(void *param, int argc, char **argv, char **azCol)
    {
        DimensionReduction* parent = reinterpret_cast<DimensionReduction*>(param);
        if (argc == 2)
        {
            SampleType::Ptr newSampleType = new SampleType(atoi(argv[0]),
                                                           String(CharPointer_UTF8(argv[1]))
                                                           );
            
            parent->sampleTypes_.add(newSampleType);
        }
        return 0;
    }
    
    
    // Static callback for a select sample segmentation query
    static int selectSampleSegmentationCallback(void *param, int argc, char **argv, char **azCol)
    {
        DimensionReduction* parent = reinterpret_cast<DimensionReduction*>(param);
        if (argc == 3)
        {
            parent->sampleSegmentations_.emplace_back(new SampleSegmentation(atol(argv[0]),
                                                                             atof(argv[1]),
                                                                             atof(argv[2])
                                                                             )
                                                      );
        }
        return 0;
    }
    
    
    // Static callback for a select sample query
    static int selectSampleCallback(void *param, int argc, char **argv, char **azCol)
    {
        DimensionReduction* parent = reinterpret_cast<DimensionReduction*>(param);
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
            
            parent->samples_.add(newSample);
        }
        return 0;
    }
    
    
    static int selectAnalysisPoolCallback(void *param, int argc, char **argv, char **azCol)
    {
        DimensionReduction* parent = reinterpret_cast<DimensionReduction*>(param);
        if (argc == 137)
        {
            parent->sampleOrder_.push_back(atoi(argv[1]));
            std::vector<Real> newRow;
            for (int i = 4; i < 137; i++)
            {
                newRow.push_back(atof(argv[i]));
            }
            parent->analysisMatrix_.push_back(newRow);
        }
        
        return 0;
    }
    
    
    
    static int selectAnalysisCallback(void *param, int argc, char **argv, char **azCol)
    {
        DimensionReduction* parent = reinterpret_cast<DimensionReduction*>(param);
        if (argc == 137)
        {
            AnalysisObject::Ptr newAnalysis = new AnalysisObject(atoi(argv[0]),
                                                                 atoi(argv[1]),
                                                                 atof(argv[2]),
                                                                 atof(argv[3]));
            newAnalysis->bark_1_mean = atof(argv[4]);
            newAnalysis->bark_2_mean = atof(argv[5]);
            newAnalysis->bark_3_mean = atof(argv[6]);
            newAnalysis->bark_4_mean = atof(argv[7]);
            newAnalysis->bark_5_mean = atof(argv[8]);
            newAnalysis->bark_6_mean = atof(argv[9]);
            newAnalysis->bark_7_mean = atof(argv[10]);
            newAnalysis->bark_8_mean = atof(argv[11]);
            newAnalysis->bark_9_mean = atof(argv[12]);
            newAnalysis->bark_10_mean = atof(argv[13]);
            newAnalysis->bark_11_mean = atof(argv[14]);
            newAnalysis->bark_12_mean = atof(argv[15]);
            newAnalysis->bark_13_mean = atof(argv[16]);
            newAnalysis->bark_14_mean = atof(argv[17]);
            newAnalysis->bark_15_mean = atof(argv[18]);
            newAnalysis->bark_16_mean = atof(argv[19]);
            newAnalysis->bark_17_mean = atof(argv[20]);
            newAnalysis->bark_18_mean = atof(argv[21]);
            newAnalysis->bark_19_mean = atof(argv[22]);
            newAnalysis->bark_20_mean = atof(argv[23]);
            newAnalysis->bark_21_mean = atof(argv[24]);
            newAnalysis->bark_22_mean = atof(argv[25]);
            newAnalysis->bark_23_mean = atof(argv[26]);
            newAnalysis->bark_24_mean = atof(argv[27]);
            newAnalysis->bark_25_mean = atof(argv[28]);
            newAnalysis->bark_26_mean = atof(argv[29]);
            newAnalysis->bark_27_mean = atof(argv[30]);
            
            newAnalysis->bark_1_dev = atof(argv[31]);
            newAnalysis->bark_2_dev = atof(argv[32]);
            newAnalysis->bark_3_dev = atof(argv[33]);
            newAnalysis->bark_4_dev = atof(argv[34]);
            newAnalysis->bark_5_dev = atof(argv[35]);
            newAnalysis->bark_6_dev = atof(argv[36]);
            newAnalysis->bark_7_dev = atof(argv[37]);
            newAnalysis->bark_8_dev = atof(argv[38]);
            newAnalysis->bark_9_dev = atof(argv[39]);
            newAnalysis->bark_10_dev = atof(argv[40]);
            newAnalysis->bark_11_dev = atof(argv[41]);
            newAnalysis->bark_12_dev = atof(argv[42]);
            newAnalysis->bark_13_dev = atof(argv[43]);
            newAnalysis->bark_14_dev = atof(argv[44]);
            newAnalysis->bark_15_dev = atof(argv[45]);
            newAnalysis->bark_16_dev = atof(argv[46]);
            newAnalysis->bark_17_dev = atof(argv[47]);
            newAnalysis->bark_18_dev = atof(argv[48]);
            newAnalysis->bark_19_dev = atof(argv[49]);
            newAnalysis->bark_20_dev = atof(argv[50]);
            newAnalysis->bark_21_dev = atof(argv[51]);
            newAnalysis->bark_22_dev = atof(argv[52]);
            newAnalysis->bark_23_dev = atof(argv[53]);
            newAnalysis->bark_24_dev = atof(argv[54]);
            newAnalysis->bark_25_dev = atof(argv[55]);
            newAnalysis->bark_26_dev = atof(argv[56]);
            newAnalysis->bark_27_dev = atof(argv[57]);
            
            newAnalysis->bark_kurtosis = atof(argv[58]);
            newAnalysis->bark_kurtosis_dev = atof(argv[59]);
            newAnalysis->bark_skewness = atof(argv[60]);
            newAnalysis->bark_skewness_dev = atof(argv[61]);
            newAnalysis->bark_spread = atof(argv[62]);
            newAnalysis->bark_spread_dev = atof(argv[63]);
            
            newAnalysis->mfcc_1_mean = atof(argv[64]);
            newAnalysis->mfcc_2_mean = atof(argv[65]);
            newAnalysis->mfcc_3_mean = atof(argv[66]);
            newAnalysis->mfcc_4_mean = atof(argv[67]);
            newAnalysis->mfcc_5_mean = atof(argv[68]);
            newAnalysis->mfcc_6_mean = atof(argv[69]);
            newAnalysis->mfcc_7_mean = atof(argv[70]);
            newAnalysis->mfcc_8_mean = atof(argv[71]);
            newAnalysis->mfcc_9_mean = atof(argv[72]);
            newAnalysis->mfcc_10_mean = atof(argv[73]);
            newAnalysis->mfcc_11_mean = atof(argv[74]);
            newAnalysis->mfcc_12_mean = atof(argv[75]);
            newAnalysis->mfcc_13_mean = atof(argv[76]);
            
            newAnalysis->mfcc_1_dev = atof(argv[77]);
            newAnalysis->mfcc_2_dev = atof(argv[78]);
            newAnalysis->mfcc_3_dev = atof(argv[79]);
            newAnalysis->mfcc_4_dev = atof(argv[80]);
            newAnalysis->mfcc_5_dev = atof(argv[81]);
            newAnalysis->mfcc_6_dev = atof(argv[82]);
            newAnalysis->mfcc_7_dev = atof(argv[83]);
            newAnalysis->mfcc_8_dev = atof(argv[84]);
            newAnalysis->mfcc_9_dev = atof(argv[85]);
            newAnalysis->mfcc_10_dev = atof(argv[86]);
            newAnalysis->mfcc_11_dev = atof(argv[87]);
            newAnalysis->mfcc_12_dev = atof(argv[88]);
            newAnalysis->mfcc_13_dev = atof(argv[89]);
            
            newAnalysis->hfc = atof(argv[90]);
            newAnalysis->pitch_salience = atof(argv[91]);
            newAnalysis->spectral_complexity = atof(argv[92]);
            newAnalysis->spectral_crest= atof(argv[93]);
            newAnalysis->spectral_decrease = atof(argv[94]);
            newAnalysis->spectral_energy = atof(argv[95]);
            newAnalysis->spectral_energyband_low = atof(argv[96]);
            newAnalysis->spectral_energyband_middle_low = atof(argv[97]);
            newAnalysis->spectral_energyband_middle_high = atof(argv[98]);
            newAnalysis->spectral_energyband_high = atof(argv[99]);
            newAnalysis->spectral_flatness_db = atof(argv[100]);
            newAnalysis->spectral_flux = atof(argv[101]);
            newAnalysis->spectral_rms = atof(argv[102]);
            newAnalysis->spectral_rolloff = atof(argv[103]);
            newAnalysis->spectral_strongpeak = atof(argv[104]);
            
            newAnalysis->hfc_dev = atof(argv[105]);
            newAnalysis->pitch_salience_dev = atof(argv[106]);
            newAnalysis->spectral_complexity_dev = atof(argv[107]);
            newAnalysis->spectral_crest_dev = atof(argv[108]);
            newAnalysis->spectral_decrease_dev = atof(argv[109]);
            newAnalysis->spectral_energy_dev = atof(argv[110]);
            newAnalysis->spectral_energyband_low_dev = atof(argv[111]);
            newAnalysis->spectral_energyband_middle_low_dev = atof(argv[112]);
            newAnalysis->spectral_energyband_middle_high_dev = atof(argv[113]);
            newAnalysis->spectral_energyband_high_dev = atof(argv[114]);
            newAnalysis->spectral_flatness_db_dev = atof(argv[115]);
            newAnalysis->spectral_flux_dev = atof(argv[116]);
            newAnalysis->spectral_rms_dev = atof(argv[117]);
            newAnalysis->spectral_rolloff_dev = atof(argv[118]);
            newAnalysis->spectral_strongpeak_dev = atof(argv[119]);
            
            newAnalysis->tristimulus_1 = atof(argv[120]);
            newAnalysis->tristimulus_2 = atof(argv[121]);
            newAnalysis->tristimulus_3 = atof(argv[122]);
            newAnalysis->tristimulus_1_dev = atof(argv[123]);
            newAnalysis->tristimulus_2_dev = atof(argv[124]);
            newAnalysis->tristimulus_3_dev = atof(argv[125]);
            
            newAnalysis->spectral_centroid = atof(argv[126]);
            newAnalysis->spectral_kurtosis = atof(argv[127]);
            newAnalysis->spectral_skewness = atof(argv[128]);
            newAnalysis->spectral_spread = atof(argv[129]);
            newAnalysis->spectral_centroid_dev = atof(argv[130]);
            newAnalysis->spectral_kurtosis_dev = atof(argv[131]);
            newAnalysis->spectral_skewness_dev = atof(argv[132]);
            newAnalysis->spectral_spread_dev = atof(argv[133]);
            
            newAnalysis->lat = atof(argv[134]);
            newAnalysis->temporal_centroid = atof(argv[135]);
            newAnalysis->rms = atof(argv[136]);
            
            parent->analyses_.add(newAnalysis);
        }
        return 0;
    }
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DimensionReduction)
    
};
