/*
  ==============================================================================

    ControlComponent.h
    Created: 20 Jun 2017 2:38:26pm
    Author:  Jordie Shier 

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "WaveformComponent.h"
#include "CustomLookAndFeel.h"
#include "Sample.h"
#include "SampleManager.h"

//==============================================================================
/*
*/
class ControlComponent :    public Component,
                            public Button::Listener,
                            public ActionBroadcaster
                            
{
public:
    ControlComponent(SampleManager* s);
    ~ControlComponent();

    void paint (Graphics&) override;
    void resized() override;
    
    void updateSelectedSample(Sample::Ptr s);
    
    // Will try to zoom-in / out if possible on selected sample
    void zoomIn();
    void zoomOut();

private:

    // Button callback
    void buttonClicked(Button* button);
    
    // Reset buttons and sample grid
    void resetState();
    
    // Pointer to sample manager
    SampleManager* sampleManager;
    
    ScopedPointer<WaveformComponent> waveformComponent;
    ScopedPointer<TextButton> newSamplesButton;
    ScopedPointer<TextButton> zoomInButton;
    ScopedPointer<TextButton> zoomOutButton;
    ScopedPointer<TextButton> resetButton;
    ScopedPointer<ComboBox> typeCombo;
    
    // Currently selected sample
    Sample::Ptr selectedSample;
    Sample::Ptr parentSample;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ControlComponent)
};
