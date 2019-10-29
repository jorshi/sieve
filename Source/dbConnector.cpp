/*
  ==============================================================================

    dbConnector.cpp
    Created: 7 Jun 2017 12:19:12am
    Author:  Jordie Shier 

  ==============================================================================
*/

#include "dbConnector.h"


DBConnector::DBConnector()
{
    
    if (sqlite3_open("/Users/jshier/Development/sample_browse.db", &db_) != SQLITE_OK) {
        Logger::writeToLog(sqlite3_errmsg(db_));
        isConnected_ = false;
        return;
    }

    isConnected_ = true;
    setupTables();
    Logger::writeToLog("Opened DB");

}


DBConnector::~DBConnector()
{
    sqlite3_close(db_);
    //std::cout << "DB Connection closed\n";
}


bool DBConnector::runCommand(juce::String command, int (*callbackFunc)(void *, int, char **, char **), void *context) const
{
    char *zErrMsg = 0;
    int rc = sqlite3_exec(db_, command.toRawUTF8(), callbackFunc, context, &zErrMsg);
    
    if (rc != SQLITE_OK)
    {
        DBG("SQL ERROR: " << zErrMsg << "\n");
        return false;
    }
    
    return true;
}


void DBConnector::setupTables()
{
    String sqlSampleFolder = "CREATE TABLE IF NOT EXISTS `sample_folders` ( " \
        "`id` INTEGER PRIMARY KEY, " \
        "`path` VARCHAR(200) NOT NULL, " \
        "`status` INT NOT NULL DEFAULT 0, " \
        "`num_samples` INT NOT NULL DEFAULT 0, " \
        "UNIQUE (`path`) " \
        ");";
    
    String sqlSampleType = "CREATE TABLE IF NOT EXISTS `sample_type` ( " \
    "`id` INTEGER PRIMARY KEY, " \
    "`name` VARCHAR(200) NOT NULL UNIQUE " \
    ");";
    
    String sqlSamples = "CREATE TABLE IF NOT EXISTS `samples` ( " \
        "`id` INTEGER PRIMARY KEY, " \
        "`name` VARCHAR(200) NOT NULL, " \
        "`path` VARCHAR(200) NOT NULL, " \
        "`start_time` DOUBLE DEFAULT NULL, " \
        "`stop_time` DOUBLE DEFAULT NULL, " \
        "`analyzed` INT(1) DEFAULT 0, " \
        "`sample_folder` INT(11) NOT NULL, "\
        "`sample_type` INT(11), "\
        "`exclude` INT(11) DEFAULT 0, " \
        "UNIQUE (`path`), " \
        "FOREIGN KEY (`sample_folder`) REFERENCES `sample_folders` (`id`) ON DELETE CASCADE, " \
        "FOREIGN KEY (`sample_type`) REFERENCES `sample_type` (`id`) ON DELETE CASCADE" \
        ");";
    
    String sqlTags = "CREATE TABLE IF NOT EXISTS `tags` ( " \
        "`id` INTEGER PRIMARY KEY, " \
        "`tag` VARCHAR(200) NOT NULL UNIQUE " \
        ");";
    
    String sqlSampleTags = "CREATE TABLE IF NOT EXISTS `sample_tags` ( " \
        "`id` INTEGER PRIMARY KEY, " \
        "`sample_id` INT(11) NOT NULL, " \
        "`tag_id` INT(11) NOT NULL, " \
        "UNIQUE (`sample_id`, `tag_id`), " \
        "FOREIGN KEY (`sample_id`) REFERENCES `samples` (`id`) ON DELETE CASCADE, " \
        "FOREIGN KEY (`tag_id`) REFERENCES `tags` (`id`) ON DELETE CASCADE " \
        ");";
    
    String sqlReducedDimension = "CREATE TABLE IF NOT EXISTS `samples_reduced` ( " \
        "`id` INTEGER PRIMARY KEY, " \
        "`sample_id` INT(11) NOT NULL, " \
        "`sample_type` INT(11) NOT NULL, " \
        "`dim_1` DOUBLE NOT NULL, " \
        "`dim_2` DOUBLE NOT NULL, " \
        "FOREIGN KEY (`sample_id`) REFERENCES `samples` (`id`) ON DELETE CASCADE " \
        ");";
    
    String sqlAnalysis = "CREATE TABLE IF NOT EXISTS `analysis` ( " \
        "`id` INTEGER PRIMARY KEY, " \
        "`sample_id` INT(11) NOT NULL, " \
        "`start` DOUBLE NOT NULL, " \
        "`length` DOUBLE NOT NULL, " \
        "bark_1_mean DOUBLE, " \
        "bark_2_mean DOUBLE, " \
        "bark_3_mean DOUBLE, " \
        "bark_4_mean DOUBLE, " \
        "bark_5_mean DOUBLE, " \
        "bark_6_mean DOUBLE, " \
        "bark_7_mean DOUBLE, " \
        "bark_8_mean DOUBLE, " \
        "bark_9_mean DOUBLE, " \
        "bark_10_mean DOUBLE, " \
        "bark_11_mean DOUBLE, " \
        "bark_12_mean DOUBLE, " \
        "bark_13_mean DOUBLE, " \
        "bark_14_mean DOUBLE, " \
        "bark_15_mean DOUBLE, " \
        "bark_16_mean DOUBLE, " \
        "bark_17_mean DOUBLE, " \
        "bark_18_mean DOUBLE, " \
        "bark_19_mean DOUBLE, " \
        "bark_20_mean DOUBLE, " \
        "bark_21_mean DOUBLE, " \
        "bark_22_mean DOUBLE, " \
        "bark_23_mean DOUBLE, " \
        "bark_24_mean DOUBLE, " \
        "bark_25_mean DOUBLE, " \
        "bark_26_mean DOUBLE, " \
        "bark_27_mean DOUBLE, " \
        "bark_1_dev DOUBLE, " \
        "bark_2_dev DOUBLE, " \
        "bark_3_dev DOUBLE, " \
        "bark_4_dev DOUBLE, " \
        "bark_5_dev DOUBLE, " \
        "bark_6_dev DOUBLE, " \
        "bark_7_dev DOUBLE, " \
        "bark_8_dev DOUBLE, " \
        "bark_9_dev DOUBLE, " \
        "bark_10_dev DOUBLE, " \
        "bark_11_dev DOUBLE, " \
        "bark_12_dev DOUBLE, " \
        "bark_13_dev DOUBLE, " \
        "bark_14_dev DOUBLE, " \
        "bark_15_dev DOUBLE, " \
        "bark_16_dev DOUBLE, " \
        "bark_17_dev DOUBLE, " \
        "bark_18_dev DOUBLE, " \
        "bark_19_dev DOUBLE, " \
        "bark_20_dev DOUBLE, " \
        "bark_21_dev DOUBLE, " \
        "bark_22_dev DOUBLE, " \
        "bark_23_dev DOUBLE, " \
        "bark_24_dev DOUBLE, " \
        "bark_25_dev DOUBLE, " \
        "bark_26_dev DOUBLE, " \
        "bark_27_dev DOUBLE, " \
        "bark_kurtosis DOUBLE, " \
        "bark_kurtosis_dev DOUBLE, " \
        "bark_skewness DOUBLE, " \
        "bark_skewness_dev DOUBLE, " \
        "bark_spread DOUBLE, " \
        "bark_spread_dev DOUBLE, " \
        "mfcc_1_mean DOUBLE, " \
        "mfcc_2_mean DOUBLE, " \
        "mfcc_3_mean DOUBLE, " \
        "mfcc_4_mean DOUBLE, " \
        "mfcc_5_mean DOUBLE, " \
        "mfcc_6_mean DOUBLE, " \
        "mfcc_7_mean DOUBLE, " \
        "mfcc_8_mean DOUBLE, " \
        "mfcc_9_mean DOUBLE, " \
        "mfcc_10_mean DOUBLE, " \
        "mfcc_11_mean DOUBLE, " \
        "mfcc_12_mean DOUBLE, " \
        "mfcc_13_mean DOUBLE, " \
        "mfcc_1_dev DOUBLE, " \
        "mfcc_2_dev DOUBLE, " \
        "mfcc_3_dev DOUBLE, " \
        "mfcc_4_dev DOUBLE, " \
        "mfcc_5_dev DOUBLE, " \
        "mfcc_6_dev DOUBLE, " \
        "mfcc_7_dev DOUBLE, " \
        "mfcc_8_dev DOUBLE, " \
        "mfcc_9_dev DOUBLE, " \
        "mfcc_10_dev DOUBLE, " \
        "mfcc_11_dev DOUBLE, " \
        "mfcc_12_dev DOUBLE, " \
        "mfcc_13_dev DOUBLE, " \
        "hfc DOUBLE, " \
        "pitch_salience DOUBLE, " \
        "spectral_complexity DOUBLE, " \
        "spectral_crest DOUBLE, " \
        "spectral_decrease DOUBLE, " \
        "spectral_energy DOUBLE, " \
        "spectral_energyband_low DOUBLE, " \
        "spectral_energyband_middle_low DOUBLE, " \
        "spectral_energyband_middle_high DOUBLE, " \
        "spectral_energyband_high DOUBLE, " \
        "spectral_flatness_db DOUBLE, " \
        "spectral_flux DOUBLE, " \
        "spectral_rms DOUBLE, " \
        "spectral_rolloff DOUBLE, " \
        "spectral_strongpeak DOUBLE, " \
        "hfc_dev DOUBLE, " \
        "pitch_salience_dev DOUBLE, " \
        "spectral_complexity_dev DOUBLE, " \
        "spectral_crest_dev DOUBLE, " \
        "spectral_decrease_dev DOUBLE, " \
        "spectral_energy_dev DOUBLE, " \
        "spectral_energyband_low_dev DOUBLE, " \
        "spectral_energyband_middle_low_dev DOUBLE, " \
        "spectral_energyband_middle_high_dev DOUBLE, " \
        "spectral_energyband_high_dev DOUBLE, " \
        "spectral_flatness_db_dev DOUBLE, " \
        "spectral_flux_dev DOUBLE, " \
        "spectral_rms_dev DOUBLE, " \
        "spectral_rolloff_dev DOUBLE, " \
        "spectral_strongpeak_dev DOUBLE, " \
        "tristimulus_1 DOUBLE, " \
        "tristimulus_2 DOUBLE, " \
        "tristimulus_3 DOUBLE, " \
        "tristimulus_1_dev DOUBLE, " \
        "tristimulus_2_dev DOUBLE, " \
        "tristimulus_3_dev DOUBLE, " \
        "spectral_centroid DOUBLE, " \
        "spectral_kurtosis DOUBLE, " \
        "spectral_skewness DOUBLE, " \
        "spectral_spread DOUBLE, " \
        "spectral_centroid_dev DOUBLE, " \
        "spectral_kurtosis_dev DOUBLE, " \
        "spectral_skewness_dev DOUBLE, " \
        "spectral_spread_dev DOUBLE, " \
        "lat DOUBLE, " \
        "temporal_centroid DOUBLE, " \
        "rms DOUBLE, " \
        "UNIQUE (`sample_id`, `start`, `length`), " \
        "FOREIGN KEY (`sample_id`) REFERENCES `samples` (`id`) ON DELETE CASCADE " \
        ");";
        
    if (    runCommand(sqlSampleFolder) &&
            runCommand(sqlSampleType) &&
            runCommand(sqlSamples) &&
            runCommand(sqlTags) &&
            runCommand(sqlSampleTags) &&
            runCommand(sqlReducedDimension) &&
            runCommand(sqlAnalysis))
    {
        //std::cout << "All tables succesfully created\n";
    }
    
    runCommand("PRAGMA foreign_keys = ON;");
    runCommand("PRAGMA busy_timeout = 1000;");
    runCommand("REPLACE INTO `sample_type` VALUES (0, 'none');");
}



