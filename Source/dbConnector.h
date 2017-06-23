/*
  ==============================================================================

    dbConnector.h
    Created: 7 Jun 2017 12:19:12am
    Author:  Jordie Shier 

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include <sqlite3.h>

class DBConnector
{
public:
    
    // Default Constructor
    DBConnector();
    
    // Default Deconstructor
    ~DBConnector();
    
    // Run a DB command
    bool runCommand(String sql, int (*callbackFunc)(void *, int, char **, char **)=callback, void* context=0) const;
    
    // Last insert ID
    int lastInsertId() const { return sqlite3_last_insert_rowid(db_); };
    
private:
    
    //===========================================================================
    // Static callback function DB executions
    static int callback(void *notUsed, int argc, char **argv, char **azColName)
    {
        for (int i=0; i < argc; i++)
        {
            std::cout << azColName << " = " << (argv[i] ? argv[i] : "NULL") << "\n";
        }
    
        std::cout << "\n";
        return 0;
    }
    
    //===========================================================================
    void setupTables();

    int setupSampleTableCallback(void *notUsed, int argc, char **argv, char **azColName);
    
    
    sqlite3* db_;
    bool isConnected_;
};
