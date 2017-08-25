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
#include "SampleType.h"

class Sample : public ReferenceCountedObject
{
public:
    
    typedef ReferenceCountedObjectPtr<Sample> Ptr;
    
    // Default Constructor
    Sample() {};
    
    // Constructor
    Sample(long long id, const String& name, const String& fullPath, double start, double stop, bool analyzed, int folder, int type=0, bool exclude=false);
    
    // Default Deconstuctor
    ~Sample() {};
    
    // Save a new sample instance
    bool save(const DBConnector& db);
    
    bool saveTagsForSample(const DBConnector& db, const Array<String>& tags);
    
    // Update database
    bool updateSave(const DBConnector& db);
    
    // Getters
    long long getId() const { return id_; };
    File& getFile() { return path_; };
    const String& getName() const { return name_; };
    AudioThumbnail& getThumbnail() { return *thumbnail_; };
    double getStartTime() { return startTime_; };
    double getStopTime() { return stopTime_; };
    Colour& getColour() { return colour_; };
    
    Sample::Ptr getParent() { return parent_; };
    ReferenceCountedArray<Sample>& getChildren() { return children_; };
    
    const String& getDisplay() { return display_; };
    
    // Setters
    void setId(const long long& id) { id_ = id; };
    void setName(const String& name) { name_ = name; };
    void setPath(const String& path) { path_ = File(path); };
    void setThumbnail(AudioThumbnail* newThumb) { thumbnail_ = newThumb; };
    void setColour(const Colour& c) { colour_ = c; };
    void setAnalyzed(const int a) { analyzed_ = a; };
    void setStartTime(const double& s) { startTime_ = s; };
    void setStopTime(const double& s) { stopTime_ = s; };
    void setExclude(bool e) { exclude_ = e; };
    void setParent(Sample::Ptr p) { parent_ = p; };
    
    void setDisplay(const String& s) { display_ = s; };
    
private:
    
    long long id_;
    String name_;
    File path_;
    double startTime_;
    double stopTime_;
    bool analyzed_;
    int folder_;
    int type_;
    bool exclude_;
    
    String display_;

    // Colour for sample pad
    Colour colour_;
    
    // Image of the waveform to show as thumbnail
    ScopedPointer<AudioThumbnail> thumbnail_;
    
    // Pointers to the parent and children samples for nesting
    Sample::Ptr parent_;
    ReferenceCountedArray<Sample> children_;
};
