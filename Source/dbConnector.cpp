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
    
    if (sqlite3_open("sample_browse.db", &db_))
    {
        isConnected_ = false;
        std::cout << "Failed to open DB";
    }
    else
    {
        isConnected_ = true;
        setupTables();
    }
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
        std::cout << "SQL ERROR: " << zErrMsg << "\n";
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
        
    if (    runCommand(sqlSampleFolder) &&
            runCommand(sqlSampleType) &&
            runCommand(sqlSamples) &&
            runCommand(sqlTags) &&
            runCommand(sqlSampleTags))
    {
        std::cout << "All tables succesfully created\n";
    }
    
    runCommand("PRAGMA foreign_keys = ON;");
    runCommand("REPLACE INTO `sample_type` VALUES (0, 'none');");
}



