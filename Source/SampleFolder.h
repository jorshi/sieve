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
    SampleFolder() {};
    SampleFolder(int id, const String& path);
    
    // Destructor
    ~SampleFolder() {};
    
    // Save current instance into database
    bool save(const DBConnector& db);
    bool updateStatus(int status, const DBConnector& db);
    
    // Get status string
    String getStatusStr();
    
    // Getters
    int getId() { return id_; };
    File& getFile() { return path_; };
    int getStatus() { return status_; };
    
    // Setters
    void setId(const int id) { id_ = id; };
    void setPath(const String& path) { path_ = File(path); };
    void setPath(const File& path) { path_ = path; };
    void setStatus(const int s) { status_ = s; };
    
private:
    
    int id_;
    File path_;
    int status_;
};
