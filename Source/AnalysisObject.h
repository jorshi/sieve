/*
  ==============================================================================

    AnalysisObject.h
    Created: 7 Aug 2017 6:59:38pm
    Author:  Jordie Shier 

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "dbConnector.h"

class AnalysisObject : public ReferenceCountedObject
{
public:
    
    typedef ReferenceCountedObjectPtr<AnalysisObject> Ptr;
    
    double bark_1_mean;
    double bark_2_mean;
    double bark_3_mean;
    double bark_4_mean;
    double bark_5_mean;
    double bark_6_mean;
    double bark_7_mean;
    double bark_8_mean;
    double bark_9_mean;
    double bark_10_mean;
    double bark_11_mean;
    double bark_12_mean;
    double bark_13_mean;
    double bark_14_mean;
    double bark_15_mean;
    double bark_16_mean;
    double bark_17_mean;
    double bark_18_mean;
    double bark_19_mean;
    double bark_20_mean;
    double bark_21_mean;
    double bark_22_mean;
    double bark_23_mean;
    double bark_24_mean;
    double bark_25_mean;
    double bark_26_mean;
    double bark_27_mean;
    
    double bark_1_dev;
    double bark_2_dev;
    double bark_3_dev;
    double bark_4_dev;
    double bark_5_dev;
    double bark_6_dev;
    double bark_7_dev;
    double bark_8_dev;
    double bark_9_dev;
    double bark_10_dev;
    double bark_11_dev;
    double bark_12_dev;
    double bark_13_dev;
    double bark_14_dev;
    double bark_15_dev;
    double bark_16_dev;
    double bark_17_dev;
    double bark_18_dev;
    double bark_19_dev;
    double bark_20_dev;
    double bark_21_dev;
    double bark_22_dev;
    double bark_23_dev;
    double bark_24_dev;
    double bark_25_dev;
    double bark_26_dev;
    double bark_27_dev;
    
    double bark_kurtosis;
    double bark_skewness;
    double bark_spread;
    
    double bark_kurtosis_dev;
    double bark_skewness_dev;
    double bark_spread_dev;
    
    double hfc;
    double hfc_dev;
    
    double mfcc_1_mean;
    double mfcc_2_mean;
    double mfcc_3_mean;
    double mfcc_4_mean;
    double mfcc_5_mean;
    double mfcc_6_mean;
    double mfcc_7_mean;
    double mfcc_8_mean;
    double mfcc_9_mean;
    double mfcc_10_mean;
    double mfcc_11_mean;
    double mfcc_12_mean;
    double mfcc_13_mean;
    
    
    double mfcc_1_dev;
    double mfcc_2_dev;
    double mfcc_3_dev;
    double mfcc_4_dev;
    double mfcc_5_dev;
    double mfcc_6_dev;
    double mfcc_7_dev;
    double mfcc_8_dev;
    double mfcc_9_dev;
    double mfcc_10_dev;
    double mfcc_11_dev;
    double mfcc_12_dev;
    double mfcc_13_dev;
    
    double pitch_salience;
    double spectral_complexity;
    double spectral_crest;
    double spectral_decrease;
    double spectral_energy;
    double spectral_energyband_low;
    double spectral_energyband_middle_low;
    double spectral_energyband_middle_high;
    double spectral_energyband_high;
    double spectral_flatness_db;
    double spectral_flux;
    double spectral_rms;
    double spectral_rolloff;
    double spectral_strongpeak;
    double zero_crossing_rate;
    double inharmonicity;
    
    double pitch_salience_dev;
    double spectral_complexity_dev;
    double spectral_crest_dev;
    double spectral_decrease_dev;
    double spectral_energy_dev;
    double spectral_energyband_low_dev;
    double spectral_energyband_middle_low_dev;
    double spectral_energyband_middle_high_dev;
    double spectral_energyband_high_dev;
    double spectral_flatness_db_dev;
    double spectral_flux_dev;
    double spectral_rms_dev;
    double spectral_rolloff_dev;
    double spectral_strongpeak_dev;
    double zero_crossing_rate_dev;
    double inharmonicity_dev;
    
    double tristimulus_1;
    double tristimulus_2;
    double tristimulus_3;
    
    double tristimulus_1_dev;
    double tristimulus_2_dev;
    double tristimulus_3_dev;
    
    double spectral_centroid;
    double spectral_kurtosis;
    double spectral_skewness;
    double spectral_spread;
    
    double spectral_centroid_dev;
    double spectral_kurtosis_dev;
    double spectral_skewness_dev;
    double spectral_spread_dev;
    
    double lat;
    double rms;
    double temporal_centroid;
    
    // Constructor
    AnalysisObject(long long id, long long sampleId, double start, double length);
    
    // Getters
    long long getId() const { return id_; };
    long long getSampleId() const { return sampleId_; };
    double getStart() const { return start_; };
    double getLength() const { return length_; };
    
    // Setters
    void setId(long long id) { id_ = id; };
    void setSampleId(long long sampleId) { sampleId_ = sampleId; };
    void setStart(double s) { start_ = s; };
    void setLength(double l) { length_ = l; };
    
    // Save analysis object into database
    bool save(const DBConnector& db);

private:
    
    long long id_;
    long long sampleId_;
    
    double start_;
    double length_;
};
