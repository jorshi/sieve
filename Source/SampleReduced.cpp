/*
  ==============================================================================

    SampleReduced.cpp
    Created: 12 Aug 2017 2:42:50am
    Author:  Jordie Shier 

  ==============================================================================
*/

#include "SampleReduced.h"

SampleReduced::SampleReduced(int id, int sampleId, int sampleType, double x, double y) :
    id_(id), sampleId_(sampleId), sampleType_(sampleType), d1_(x), d2_(y)
{
    
}

SampleReduced::SampleReduced(const SampleReduced& s)
{
    id_ = s.id_;
    sampleId_ = s.sampleId_;
    sampleType_ = s.sampleType_;
    d1_ = s.d1_;
    d2_ = s.d2_;
    sample_ = s.sample_;
}


bool SampleReduced::save(const DBConnector& db)
{
    String sql = "INSERT INTO `samples_reduced` (`sample_id`, `sample_type`, `dim_1`, `dim_2`) VALUES (" + \
        String(sampleId_) + ", " + \
        String(sampleType_) + ", " + \
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
