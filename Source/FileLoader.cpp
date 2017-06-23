/*
  ==============================================================================

    FileLoader.cpp
    Created: 17 Mar 2017 1:20:56pm
    Author:  Jordie Shier 

  ==============================================================================
*/

#include "FileLoader.h"

FileLoader::FileLoader()
{
    formatManager.registerFormat(new WavAudioFormat, true);
}

FileLoader::~FileLoader()
{
    
}


File* FileLoader::getNewAudioFile()
{
    // Get new audio file
    FileChooser chooser ("Choose audio file ...",
                         File::nonexistent,
                         "*.wav");
    
    // Open file and make sure it is a valid audio file
    if(chooser.browseForFileToOpen())
    {
        File* file = new File(chooser.getResult());
        if (canReadFile(*file))
        {
            return file;
        }
    }
    
    return nullptr;
}


bool FileLoader::canReadFile(juce::File& file)
{
    ScopedPointer<AudioFormatReader> reader = formatManager.createReaderFor(file);
    
    if (reader != nullptr)
    {
        return true;
    }
    
    return false;
}


bool FileLoader::fileExtensionOkay(const juce::String &filename)
{
    File file(filename);
    const String extensions = formatManager.getWildcardForAllFormats().removeCharacters("*");
    return file.hasFileExtension(extensions);
}


AudioFormatReader* FileLoader::getAudioReader(File& file)
{
    AudioFormatReader* reader = formatManager.createReaderFor(file);
    return reader;
}
