/*
  ==============================================================================

    FileLoader.h
    Created: 17 Mar 2017 1:20:56pm
    Author:  Jordie Shier 

  ==============================================================================
*/

#ifndef FILELOADER_H_INCLUDED
#define FILELOADER_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"

class FileLoader
{
public:
    
    // Constructor
    FileLoader();
    
    // Destructor
    ~FileLoader();
    
    // Loads a new audio file
    File* getNewAudioFile();
    
    // Confirms whether we can read this file or not
    bool canReadFile(File& file);
    
    // Checks if we can read a given file extension
    bool fileExtensionOkay(const String& filename);
    
    // Create a new audio format reader for a file
    AudioFormatReader* getAudioReader(File& file);
    
    // Get reference to format manager
    AudioFormatManager& getFormatManager() { return formatManager; };
    
private:
    
    AudioFormatManager formatManager;
};


#endif  // FILELOADER_H_INCLUDED
