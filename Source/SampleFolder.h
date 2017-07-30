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

class SampleFolder
{
public:
    
    SampleFolder() {};
    SampleFolder(int id, const String& path);
    
    ~SampleFolder() {};
    
    bool save(const DBConnector& db);
    
    // Getters
    int getId() { return id_; };
    File& getFile() { return path_; };
    
private:
    
    int id_;
    File path_;
                
};
