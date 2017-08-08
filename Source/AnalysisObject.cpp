/*
  ==============================================================================

    AnalysisObject.cpp
    Created: 7 Aug 2017 6:59:38pm
    Author:  Jordie Shier 

  ==============================================================================
*/

#include "AnalysisObject.h"

AnalysisObject::AnalysisObject(long long id, long long sampleId, double start, double length) : id_(id),
    sampleId_(sampleId), start_(start), length_(length)
{
    
}

bool AnalysisObject::save(const DBConnector& db)
{
    String sql = "REPLACE INTO `analysis` (" \
        "sample_id, start, length, " \
        "bark_1_mean, bark_2_mean, bark_3_mean, bark_4_mean, bark_5_mean, bark_6_mean, bark_7_mean, bark_8_mean, bark_9_mean, bark_10_mean, " \
        "bark_11_mean, bark_12_mean, bark_13_mean, bark_14_mean, bark_15_mean, bark_16_mean, bark_17_mean, bark_18_mean, bark_19_mean, " \
        "bark_20_mean, bark_21_mean, bark_22_mean, bark_23_mean, bark_24_mean, bark_25_mean, bark_26_mean, bark_27_mean, " \
        "bark_1_dev, bark_2_dev, bark_3_dev, bark_4_dev, bark_5_dev, bark_6_dev, bark_7_dev, bark_8_dev, bark_9_dev, bark_10_dev, " \
        "bark_11_dev, bark_12_dev, bark_13_dev, bark_14_dev, bark_15_dev, bark_16_dev, bark_17_dev, bark_18_dev, bark_19_dev, bark_20_dev, " \
        "bark_21_dev, bark_22_dev, bark_23_dev, bark_24_dev, bark_25_dev, bark_26_dev, bark_27_dev, " \
        "bark_kurtosis, bark_kurtosis_dev, bark_skewness, bark_skewness_dev, bark_spread, bark_spread_dev, " \
        "mfcc_1_mean, mfcc_2_mean, mfcc_3_mean, mfcc_4_mean, mfcc_5_mean, mfcc_6_mean, mfcc_7_mean, mfcc_8_mean, mfcc_9_mean, mfcc_10_mean, " \
        "mfcc_11_mean, mfcc_12_mean, mfcc_13_mean, " \
        "mfcc_1_dev, mfcc_2_dev, mfcc_3_dev, mfcc_4_dev, mfcc_5_dev, mfcc_6_dev, mfcc_7_dev, mfcc_8_dev, mfcc_9_dev, mfcc_10_dev, " \
        "mfcc_11_dev, mfcc_12_dev, mfcc_13_dev, " \
        "hfc, pitch_salience, spectral_complexity, spectral_crest, spectral_decrease, spectral_energy, spectral_energyband_low, " \
        "spectral_energyband_middle_low, spectral_energyband_middle_high, spectral_energyband_high, spectral_flatness_db, spectral_flux, " \
        "spectral_rms, spectral_rolloff, spectral_strongpeak, " \
        "hfc_dev, pitch_salience_dev, spectral_complexity_dev, spectral_crest_dev, spectral_decrease_dev, spectral_energy_dev, " \
        "spectral_energyband_low_dev, spectral_energyband_middle_low_dev, spectral_energyband_middle_high_dev, spectral_energyband_high_dev, " \
        "spectral_flatness_db_dev, spectral_flux_dev, spectral_rms_dev, spectral_rolloff_dev, spectral_strongpeak_dev, " \
        "tristimulus_1, tristimulus_2, tristimulus_3, " \
        "tristimulus_1_dev, tristimulus_2_dev, tristimulus_3_dev, " \
        "spectral_centroid, spectral_kurtosis, spectral_skewness, spectral_spread, " \
        "spectral_centroid_dev, spectral_kurtosis_dev, spectral_skewness_dev, spectral_spread_dev, " \
        "lat, temporal_centroid, rms) " \
        "VALUES (" + \
        String(sampleId_) + ", " + \
        String(start_) + ", " + \
        String(length_) + ", " + \
        String(bark_1_mean) + ", " + \
        String(bark_2_mean) + ", " + \
        String(bark_3_mean) + ", " + \
        String(bark_4_mean) + ", " + \
        String(bark_5_mean) + ", " + \
        String(bark_6_mean) + ", " + \
        String(bark_7_mean) + ", " + \
        String(bark_8_mean) + ", " + \
        String(bark_9_mean) + ", " + \
        String(bark_10_mean) + ", " + \
        String(bark_11_mean) + ", " + \
        String(bark_12_mean) + ", " + \
        String(bark_13_mean) + ", " + \
        String(bark_14_mean) + ", " + \
        String(bark_15_mean) + ", " + \
        String(bark_16_mean) + ", " + \
        String(bark_17_mean) + ", " + \
        String(bark_18_mean) + ", " + \
        String(bark_19_mean) + ", " + \
        String(bark_20_mean) + ", " + \
        String(bark_21_mean) + ", " + \
        String(bark_22_mean) + ", " + \
        String(bark_23_mean) + ", " + \
        String(bark_24_mean) + ", " + \
        String(bark_25_mean) + ", " + \
        String(bark_26_mean) + ", " + \
        String(bark_27_mean) + ", " + \
        String(bark_1_dev) + ", " + \
        String(bark_2_dev) + ", " + \
        String(bark_3_dev) + ", " + \
        String(bark_4_dev) + ", " + \
        String(bark_5_dev) + ", " + \
        String(bark_6_dev) + ", " + \
        String(bark_7_dev) + ", " + \
        String(bark_8_dev) + ", " + \
        String(bark_9_dev) + ", " + \
        String(bark_10_dev) + ", " + \
        String(bark_11_dev) + ", " + \
        String(bark_12_dev) + ", " + \
        String(bark_13_dev) + ", " + \
        String(bark_14_dev) + ", " + \
        String(bark_15_dev) + ", " + \
        String(bark_16_dev) + ", " + \
        String(bark_17_dev) + ", " + \
        String(bark_18_dev) + ", " + \
        String(bark_19_dev) + ", " + \
        String(bark_20_dev) + ", " + \
        String(bark_21_dev) + ", " + \
        String(bark_22_dev) + ", " + \
        String(bark_23_dev) + ", " + \
        String(bark_24_dev) + ", " + \
        String(bark_25_dev) + ", " + \
        String(bark_26_dev) + ", " + \
        String(bark_27_dev) + ", " + \
        String(bark_kurtosis) + ", " + \
        String(bark_kurtosis_dev) + ", " + \
        String(bark_skewness) + ", " + \
        String(bark_skewness_dev) + ", " + \
        String(bark_spread) + ", " + \
        String(bark_spread_dev) + ", " + \
        String(mfcc_1_mean) + ", " + \
        String(mfcc_2_mean) + ", " + \
        String(mfcc_3_mean) + ", " + \
        String(mfcc_4_mean) + ", " + \
        String(mfcc_5_mean) + ", " + \
        String(mfcc_6_mean) + ", " + \
        String(mfcc_7_mean) + ", " + \
        String(mfcc_8_mean) + ", " + \
        String(mfcc_9_mean) + ", " + \
        String(mfcc_10_mean) + ", " + \
        String(mfcc_11_mean) + ", " + \
        String(mfcc_12_mean) + ", " + \
        String(mfcc_13_mean) + ", " + \
        String(mfcc_1_dev) + ", " + \
        String(mfcc_2_dev) + ", " + \
        String(mfcc_3_dev) + ", " + \
        String(mfcc_4_dev) + ", " + \
        String(mfcc_5_dev) + ", " + \
        String(mfcc_6_dev) + ", " + \
        String(mfcc_7_dev) + ", " + \
        String(mfcc_8_dev) + ", " + \
        String(mfcc_9_dev) + ", " + \
        String(mfcc_10_dev) + ", " + \
        String(mfcc_11_dev) + ", " + \
        String(mfcc_12_dev) + ", " + \
        String(mfcc_13_dev) + ", " + \
        String(hfc) + ", " + \
        String(pitch_salience) + ", " + \
        String(spectral_complexity) + ", " + \
        String(spectral_crest) + ", " + \
        String(spectral_decrease) + ", " + \
        String(spectral_energy) + ", " + \
        String(spectral_energyband_low) + ", " + \
        String(spectral_energyband_middle_low) + ", " + \
        String(spectral_energyband_middle_high) + ", " + \
        String(spectral_energyband_high) + ", " + \
        String(spectral_flatness_db) + ", " + \
        String(spectral_flux) + ", " + \
        String(spectral_rms) + ", " + \
        String(spectral_rolloff) + ", " + \
        String(spectral_strongpeak) + ", " + \
        String(hfc_dev) + ", " + \
        String(pitch_salience_dev) + ", " + \
        String(spectral_complexity_dev) + ", " + \
        String(spectral_crest_dev) + ", " + \
        String(spectral_decrease_dev) + ", " + \
        String(spectral_energy_dev) + ", " + \
        String(spectral_energyband_low_dev) + ", " + \
        String(spectral_energyband_middle_low_dev) + ", " + \
        String(spectral_energyband_middle_high_dev) + ", " + \
        String(spectral_energyband_high_dev) + ", " + \
        String(spectral_flatness_db_dev) + ", " + \
        String(spectral_flux_dev) + ", " + \
        String(spectral_rms_dev) + ", " + \
        String(spectral_rolloff_dev) + ", " + \
        String(spectral_strongpeak_dev) + ", " + \
        String(tristimulus_1) + ", " + \
        String(tristimulus_2) + ", " + \
        String(tristimulus_3) + ", " + \
        String(tristimulus_1_dev) + ", " + \
        String(tristimulus_2_dev) + ", " + \
        String(tristimulus_3_dev) + ", " + \
        String(spectral_centroid) + ", " + \
        String(spectral_kurtosis) + ", " + \
        String(spectral_skewness) + ", " + \
        String(spectral_spread) + ", " + \
        String(spectral_centroid_dev) + ", " + \
        String(spectral_kurtosis_dev) + ", " + \
        String(spectral_skewness_dev) + ", " + \
        String(spectral_spread_dev) + ", " + \
        String(lat) + ", " + \
        String(temporal_centroid) + ", " + \
        String(rms) + \
        ");";
    
    if (db.runCommand(sql))
    {
        id_ = db.lastInsertId();
        return true;
    }
    
    return false;
}
