/*
  ==============================================================================

    Sample.cpp
    Created: 15 Jun 2017 12:36:16pm
    Author:  Jordie Shier 

  ==============================================================================
*/

#include "Sample.h"

// Full Constructor
Sample::Sample(int id, const String& name, const String& fullPath, double start, double stop, bool analyzed, int folder, int type) :
    id_(id), name_(name), path_(fullPath), startTime_(start), stopTime_(stop), analyzed_(analyzed), folder_(folder), type_(type)
{
    
}


bool Sample::save(const DBConnector &db)
{
    
    db.runCommand("BEGIN;");
    
    String sql = "INSERT INTO `samples` (`name`, `path`, `start_time`, `stop_time`, `analyzed`, `sample_folder`, `sample_type`) " \
        "VALUES ('" + name_.replace("'", "''") + \
        "', '" + path_.getFullPathName().replace("'", "''") + \
        "', " + String(startTime_) + \
        ", " + String(stopTime_) + \
        ", " + String(0) + \
        ", " + String(folder_) + \
        ", " + String(type_) + \
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
