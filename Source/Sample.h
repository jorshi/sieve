/*
  ==============================================================================

    Sample.h
    Created: 15 Jun 2017 12:36:16pm
    Author:  Jordie Shier 

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "dbConnector.h"

class Sample : public ReferenceCountedObject
{
public:
    
    typedef ReferenceCountedObjectPtr<Sample> Ptr;
    
    // Default Constructor
    Sample() {};
    
    // Constructor
    Sample(int id, const String& name, const String& fullPath, double start, double stop, bool analyzed, int folder);
    
    // Default Deconstuctor
    ~Sample() {};
    
    // Save a new sample instance
    bool save(const DBConnector& db);
    
    bool saveTagsForSample(const DBConnector& db, const Array<String>& tags);
    
    // Getters
    int getId() const { return id_; };
    File& getFile() { return path_; };
    const String& getName() const { return name_; };
    AudioThumbnail& getThumbnail() { return *thumbnail_; };
    Colour& getColour() { return colour_; };
    
    // Setters
    void setId(const int& id) { id_ = id; };
    void setName(const String& name) { name_ = name; };
    void setPath(const String& path) { path_ = File(path); };
    void setThumbnail(AudioThumbnail* newThumb) { thumbnail_ = newThumb; };
    void setColour(const Colour& c) { colour_ = c; };
    void setAnalyzed(const int a) { analyzed_ = a; };
    
private:
    
    int id_;
    String name_;
    File path_;
    double startTime_;
    double stopTime_;
    bool analyzed_;
    int folder_;
    Colour colour_;
    
    ScopedPointer<AudioThumbnail> thumbnail_;
};
