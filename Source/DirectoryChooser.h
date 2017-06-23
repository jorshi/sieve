/*
  ==============================================================================

    DirectoryChooser.h
    Created: 12 Jun 2017 4:35:56pm
    Author:  Jordie Shier 

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

class DirectoryChooser
{
public:
    
    // Default Constructor
    DirectoryChooser();
    
    // Default Deconstructor
    ~DirectoryChooser() {};
    
    // Get a new directory
    bool getDirectory(File&);
    
    
private:
    
    ScopedPointer<FileChooser> fileChooser_;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DirectoryChooser);
};
