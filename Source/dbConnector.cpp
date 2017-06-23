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
        //std::cout << "DB connection established\n";
        
        // Setup Tables
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
        //std::cout << "SQL ERROR: " << zErrMsg << "\n";
        return false;
    }
    
    return true;
}


void DBConnector::setupTables()
{
    // Need to setup tables
    String sqlSamples = "CREATE TABLE IF NOT EXISTS`samples` ( " \
        "`id` INTEGER PRIMARY KEY, " \
        "`name` VARCHAR(200) NOT NULL, " \
        "`path` VARCHAR(200) NOT NULL, " \
        "`start_time` DOUBLE DEFAULT NULL, " \
        "`stop_time` DOUBLE DEFAULT NULL, " \
        "UNIQUE (`path`) " \
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
        
    if (runCommand(sqlSamples) &&
            runCommand(sqlTags) &&
            runCommand(sqlSampleTags))
    {
        //std::cout << "All tables succesfully created\n";
    }
}



