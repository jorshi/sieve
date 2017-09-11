/*
  ==============================================================================

    WaveformComponent.h
    Created: 16 Jun 2017 12:36:19am
    Author:  Jordie Shier 

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "CustomLookAndFeel.h"
#include "Sample.h"

//==============================================================================
/*
*/
class WaveformComponent    : public Component,
                             public DragAndDropContainer
{
public:
    WaveformComponent();
    ~WaveformComponent();

    void paint (Graphics&) override;
    void resized() override;
    
    void updateSampleAndDraw(Sample::Ptr s);
    
    // Mouse drag on the waveform component for drag and drop of file
    void mouseDrag(const MouseEvent& event) override;

private:
    
    CustomLookAndFeel lookAndFeel;
    Sample::Ptr sample;
    
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (WaveformComponent)
};
