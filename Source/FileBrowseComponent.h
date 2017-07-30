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

//==============================================================================
/*
*/
class FileBrowseComponent    : public Component,
                               public ButtonListener,
                               public ActionBroadcaster
{
public:
    FileBrowseComponent(SampleManager* m);
    ~FileBrowseComponent();

    void paint (Graphics&) override;
    void resized() override;

private:
    
    void buttonClicked(Button* button) override;
    
    ScopedPointer<TextButton> fileBrowser;
    SampleManager* manager;
    
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FileBrowseComponent)
};
