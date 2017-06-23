/*
  ==============================================================================

    SamplePad.h
    Created: 2 Jun 2017 5:54:47pm
    Author:  Jordie Shier 

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

//==============================================================================
/*
*/
class SamplePad    : public Component
{
public:
    SamplePad();
    ~SamplePad();

    void paint (Graphics&) override;
    void resized() override;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SamplePad)
};
