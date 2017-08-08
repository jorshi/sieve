/*
  ==============================================================================

    SampleFolder.cpp
    Created: 29 Jul 2017 7:55:21pm
    Author:  Jordie Shier 

  ==============================================================================
*/

#include "SampleFolder.h"

SampleFolder::SampleFolder(int id, const String& path) : id_(id), path_(path), status_(1)
{
    
}

bool SampleFolder::save(const DBConnector &db)
{
    
    db.runCommand("BEGIN;");
    
    String sql = "INSERT INTO `sample_folders` (`path`, `status`, `num_samples`) " \
        "VALUES ('" + path_.getFullPathName().replace("'", "''") + "'" + \
        ", " + String(status_) + \
        ", " + String(numSamples_) + \
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

bool SampleFolder::updateStatus(int status, const DBConnector &db)
{
    // Start DB transaction
    db.runCommand("BEGIN;");
    
    String sql = "UPDATE `sample_folders` " \
    "SET `status` = " + String(status) + \
    " WHERE `id` = " + String(id_) + ";";
    
    if (db.runCommand(sql))
    {
        db.runCommand("COMMIT;");
        status_ = status;
        return true;
    }
    
    db.runCommand("ROLLBACK;");
    return false;
}

bool SampleFolder::update(const DBConnector &db)
{
    String sql = "UPDATE `sample_folders` SET " \
        "`path` = '" + path_.getFullPathName().replace("'", "''") + "', " + \
        "`status` = " + String(status_) + ", " + \
        "`num_samples` = " + String(numSamples_) + " " \
        "WHERE `id` = " + String(id_) + ";";
    
    return db.runCommand(sql);
}

String SampleFolder::getStatusStr()
{
    switch (status_)
    {
        case 1:
            return "Loading";
        case 2:
            return "Analyzing";
        case 3:
            return "Analyzed";
        default:
            return "None";
    }
}
