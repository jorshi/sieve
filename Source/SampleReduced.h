/*
  ==============================================================================

    SampleReduced.h
    Created: 12 Aug 2017 2:42:50am
    Author:  Jordie Shier 

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "dbConnector.h"
#include "SampleType.h"
#include "Sample.h"

class SampleReduced : public ReferenceCountedObject
{
public:
    
    typedef ReferenceCountedObjectPtr<SampleReduced> Ptr;
    
    SampleReduced(int id, int sampleId, double x, double y);
    ~SampleReduced() {};
    
    // Getters
    int getId() { return id_; };
    int getSampleId() { return sampleId_; };
    double getX() { return d1_; };
    double getY() { return d2_; };
    
    // Setters
    void setId(const int& id) { id_ = id; };
    void setSampleId(const int& id) { sampleId_ = id; };
    void setX(const double& x) { d1_ = x; };
    void setY(const double& y) { d2_ = y; };
    
    // Save instance into database
    bool save(const DBConnector& db);
    
private:
    
    int id_;
    int sampleId_;
    
    // The interface shows X and Y, but keep the dimensions general in here
    // in case we want to add more later on
    double d1_;
    double d2_;
};
