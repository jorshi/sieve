/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"


//==============================================================================
SampleBrowserAudioProcessorEditor::SampleBrowserAudioProcessorEditor (SampleBrowserAudioProcessor& p)
    : AudioProcessorEditor (&p), processor (p), sampleManager(p.getSampleManager())
{
    setLookAndFeel(&customLookAndFeel);
    
    header = new HeaderComponent;
    addAndMakeVisible(header);
    
    main = new MainComponent(sampleManager);
    main->addActionListener(this);
    addAndMakeVisible(main);
    
    setSize (650, 675);
}

SampleBrowserAudioProcessorEditor::~SampleBrowserAudioProcessorEditor()
{
}

//==============================================================================
void SampleBrowserAudioProcessorEditor::paint (Graphics& g)
{
    
}

void SampleBrowserAudioProcessorEditor::resized()
{
    header->setBounds(0, 0, 650, 50);
    main->setBounds(0, 50, 650, 625);
}

//==============================================================================
void SampleBrowserAudioProcessorEditor::buttonClicked(juce::Button *button)
{
}


void SampleBrowserAudioProcessorEditor::actionListenerCallback(const juce::String &message)
{
    if (message == "update_grid")
    {
        processor.loadSamplerSounds();
    }
    else
    {
        int pad = message.getIntValue();
        processor.triggerSound(pad);
    }
}
