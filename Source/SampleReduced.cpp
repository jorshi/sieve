/*
  ==============================================================================

    SampleReduced.cpp
    Created: 12 Aug 2017 2:42:50am
    Author:  Jordie Shier 

  ==============================================================================
*/

#include "SampleReduced.h"

SampleReduced::SampleReduced(int id, int sampleId, double x, double y) :
    id_(id), sampleId_(sampleId), d1_(x), d2_(y)
{
    
}

bool SampleReduced::save(const DBConnector& db)
{
    String sql = "INSERT INTO `samples_reduced` (`sample_id`, `dim_1`, `dim_2`) VALUES (" + \
        String(sampleId_) + ", " + \
        String(d1_) + ", " + \
        String(d2_) + \
        ");";
    
    if (db.runCommand(sql))
    {
        id_ = db.lastInsertId();
        return true;
    }
    
    return false;
}
