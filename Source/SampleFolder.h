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
    
    // Getters
    int getId() { return id_; };
    File& getFile() { return path_; };
    
    // Setters
    void setId(const int id) { id_ = id; };
    void setPath(const String& path) { path_ = File(path); };
    void setPath(const File& path) { path_ = path; };
    
private:
    
    int id_;
    File path_;
};
