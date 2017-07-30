/*
  ==============================================================================

    SampleFolder.cpp
    Created: 29 Jul 2017 7:55:21pm
    Author:  Jordie Shier 

  ==============================================================================
*/

#include "SampleFolder.h"

SampleFolder::SampleFolder(int id, const String& path) : id_(id), path_(path)
{
    
}

bool SampleFolder::save(const DBConnector &db)
{
    
    db.runCommand("BEGIN;");
    
    String sql = "INSERT INTO `sample_folders` (`path`) " \
        "VALUES ('" + path_.getFullPathName().replace("'", "''") + "'" + \
        ");";
    
    if (db.runCommand(sql))
    {
        id_ = db.lastInsertId();
        db.runCommand("COMMIT;");
        return true;
    }
    
    db.runCommand("ROLLBACK;");
    return false;
}
