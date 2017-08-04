/*
  ==============================================================================

    SampleType.cpp
    Created: 3 Aug 2017 11:12:47am
    Author:  Jordie Shier 

  ==============================================================================
*/

#include "SampleType.h"

SampleType::SampleType(int id, const String& name) : id_(id), name_(name)
{

}

bool SampleType::save(const DBConnector &db)
{
    db.runCommand("BEGIN;");
    
    String sql = "INSERT INTO `sample_type` (`name`) VALUES ('" + name_ + "');";
    
    if (db.runCommand(sql))
    {
        id_ = db.lastInsertId();
        db.runCommand("COMMIT;");
        return true;
    }
    
    db.runCommand("ROLLBACK;");
    return false;
}
