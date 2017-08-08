/*
  ==============================================================================

    SampleFolder.h
    Created: 29 Jul 2017 7:55:21pm
    Author:  Jordie Shier 

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "dbConnector.h"


class SampleFolder : public ReferenceCountedObject
{
public:
    
    typedef ReferenceCountedObjectPtr<SampleFolder> Ptr;
    
    // Constructors
    SampleFolder() : numSamples_(0), percentAnalyzed_(0) {};
    SampleFolder(int id, const String& path);
    
    // Destructor
    ~SampleFolder() {};
    
    // Save current instance into database
    bool save(const DBConnector& db);
    bool updateStatus(int status, const DBConnector& db);
    bool update(const DBConnector& db);
    
    // Get status string
    String getStatusStr();
    
    // Increment the number of samples
    void incrementSamples() { ++numSamples_; };
    
    // Getters
    long long getId() { return id_; };
    File& getFile() { return path_; };
    int getStatus() { return status_; };
    int getNumSamples() { return numSamples_; };
    double getPercentAnalyzed() { return percentAnalyzed_; };
    
    // Setters
    void setId(const long long id) { id_ = id; };
    void setPath(const String& path) { path_ = File(path); };
    void setPath(const File& path) { path_ = path; };
    void setStatus(const int s) { status_ = s; };
    void setNumSamples(const int n) { numSamples_ = n; };
    void setPercentAnalyzed(const double p) { percentAnalyzed_ = p; };
    
private:
    
    long long id_;
    File path_;
    int status_;
    int numSamples_;
    
    double percentAnalyzed_;
};
