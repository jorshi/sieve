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
    
    String sql = "INSERT INTO `sample_folders` (`path`, `status`) " \
        "VALUES ('" + path_.getFullPathName().replace("'", "''") + "'" + \
        ", " + String(status_) + \
        ");";
    
    std::cout << sql << "\n";
    
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
    status_ = status;
    
    // Start DB transaction
    db.runCommand("BEGIN;");
    
    String sql = "UPDATE `sample_folders` " \
    "SET `status` = " + String(status_) + \
    " WHERE `id` = " + String(id_) + ";";
    
    if (db.runCommand(sql))
    {
        db.runCommand("COMMIT;");
        return true;
    }
    
    db.runCommand("ROLLBACK;");
    return false;
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
