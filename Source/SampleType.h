/*
  ==============================================================================

    SampleType.h
    Created: 3 Aug 2017 11:12:47am
    Author:  Jordie Shier 

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "dbConnector.h"

class SampleType : public ReferenceCountedObject
{
public:
    
    // Pointer for this sample type objects
    typedef ReferenceCountedObjectPtr<SampleType> Ptr;
    
    // Constructors
    SampleType() {};
    SampleType(int id, const String& name);
    
    // Deconstructor
    ~SampleType() {};
    
    // Getters
    long long getId() { return id_; };
    const String& getName() { return name_; };
    
    // Save this instance into the database
    bool save(const DBConnector& db);
    
private:
    
    long long id_;
    String name_;
};
