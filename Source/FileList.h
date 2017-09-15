/*
  ==============================================================================

    FileList.h
    Created: 12 Sep 2017 1:38:27pm
    Author:  Jordie Shier 

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "SampleManager.h"
#include "CustomLookAndFeel.h"

//==============================================================================
/*
*/
class FileList    : public Component
{
public:
    FileList(SampleManager* m);
    ~FileList();

    void paint (Graphics&) override;
    void resized() override;
    
    int getRequiredHeight();

private:
    SampleManager* manager_;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FileList)
};
