/*
  ==============================================================================

    FileBrowseComponent.h
    Created: 29 Jul 2017 5:29:39pm
    Author:  Jordie Shier 

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "CustomLookAndFeel.h"
#include "SampleManager.h"
#include "FileList.h"

//==============================================================================
/*
*/
class FileBrowseComponent    : public Component,
                               public ButtonListener,
                               public ActionBroadcaster,
                               public Timer
{
public:
    FileBrowseComponent(SampleManager* m);
    ~FileBrowseComponent();

    void paint (Graphics&) override;
    void resized() override;
    
    void timerCallback() override;

private:
    
    void buttonClicked(Button* button) override;
    
    // Resize the file list if there are more files to show
    void resizeFileList();
    
    SampleManager* manager;
    
    ScopedPointer<TextButton> fileBrowser;
    ScopedPointer<FileList> fileList;
    ScopedPointer<Viewport> fileViewPort;
    
    int currentFileListSize;
    

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FileBrowseComponent)
};
