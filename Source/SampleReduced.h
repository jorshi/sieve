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
    
    SampleReduced(int id, int sampleId, int sampleType, double x, double y);
    ~SampleReduced() {};
    
    // Copy Constructor
    SampleReduced(const SampleReduced& s);
    
    // Getters
    int getId() { return id_; };
    int getSampleId() { return sampleId_; };
    int getSampleTypeId() { return sampleType_; }
    double getX() { return d1_; };
    double getY() { return d2_; };
    Sample::Ptr getSamplePtr() { return sample_; };
    
    // Setters
    void setId(const int& id) { id_ = id; };
    void setSampleId(const int& id) { sampleId_ = id; };
    void setX(const double& x) { d1_ = x; };
    void setY(const double& y) { d2_ = y; };
    void setSamplePtr(Sample::Ptr s) { sample_ = s; };
    
    // Save instance into database
    bool save(const DBConnector& db);
    
private:
    
    long long id_;
    long long sampleId_;
    long long sampleType_;
    
    // The interface shows X and Y, but keep the dimensions general in here
    // in case we want to add more later on
    double d1_;
    double d2_;
    
    // For utility purposes can store a pointer to the sample this refers to
    Sample::Ptr sample_;
};
