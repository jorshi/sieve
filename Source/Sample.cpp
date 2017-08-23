/*
  ==============================================================================

    Sample.cpp
    Created: 15 Jun 2017 12:36:16pm
    Author:  Jordie Shier 

  ==============================================================================
*/

#include "Sample.h"

// Full Constructor
Sample::Sample(long long id, const String& name, const String& fullPath, double start, double stop, bool analyzed, int folder, int type, bool exclude) :
    id_(id), name_(name), path_(fullPath), startTime_(start), stopTime_(stop), analyzed_(analyzed), folder_(folder), type_(type), exclude_(exclude)
{
    parent_ = nullptr;
}


bool Sample::save(const DBConnector &db)
{
    
    db.runCommand("BEGIN;");
    
    String sql = "INSERT INTO `samples` (`name`, `path`, `start_time`, `stop_time`, `analyzed`, `sample_folder`, `sample_type`, `exclude`) " \
        "VALUES ('" + name_.replace("'", "''") + \
        "', '" + path_.getFullPathName().replace("'", "''") + \
        "', " + String(startTime_) + \
        ", " + String(stopTime_) + \
        ", " + String(0) + \
        ", " + String(folder_) + \
        ", " + String(type_) + \
        ", " + String(exclude_) + \
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

bool Sample::saveTagsForSample(const DBConnector &db, const Array<juce::String> &tags)
{
    // Add all the tags
    for (auto tag = tags.begin(); tag != tags.end(); ++tag)
    {
        String sql = "INSERT INTO `tags` (`tag`) VALUES('" + (*tag).toLowerCase() + "');";
        
        if (db.runCommand(sql))
        {
            long long tagId = db.lastInsertId();
        }
    }
    
    return true;
}


bool Sample::updateSave(const DBConnector &db)
{    
    String sql = "UPDATE `samples` SET " \
        "name = '" + name_.replace("'", "''") + "', " + \
        "start_time = " + String(startTime_) + ", " + \
        "stop_time = " + String(stopTime_) + ", " + \
        "analyzed = " + String(int(analyzed_)) + ", " + \
        "sample_folder = " + String(folder_) + ", " + \
        "sample_type = " + String(type_) + ", " + \
        "exclude = " + String(exclude_) + " " + \
        "WHERE id = " + String(id_) + ";";
    
    return db.runCommand(sql);
 }
