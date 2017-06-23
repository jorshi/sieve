/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "PluginProcessor.h"
#include "CustomLookAndFeel.h"
#include "SampleGrid.h"
#include "DirectoryChooser.h"
#include "SampleManager.h"
#include "Sample.h"

#include "WaveformComponent.h"
#include "HeaderComponent.h"
#include "MainComponent.h"


//==============================================================================
/**
*/
class SampleBrowserAudioProcessorEditor  : public AudioProcessorEditor,
                                           public ButtonListener,
                                           public ActionListener
{
public:
    SampleBrowserAudioProcessorEditor (SampleBrowserAudioProcessor&);
    ~SampleBrowserAudioProcessorEditor();

    //==============================================================================
    void paint (Graphics&) override;
    void resized() override;

private:
    // Button callback
    void buttonClicked(Button* button) override;
    
    // Action callback
    void actionListenerCallback(const String& message) override;
    
    // Reference to processor
    SampleBrowserAudioProcessor& processor;
    
    // Sample Manager
    SampleManager* sampleManager;
    CustomLookAndFeel customLookAndFeel;
    
    // Components
    ScopedPointer<HeaderComponent> header;
    ScopedPointer<MainComponent> main;




    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SampleBrowserAudioProcessorEditor)
};
