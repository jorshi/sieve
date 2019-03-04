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
    header->addActionListener(this);
    addAndMakeVisible(header);
    
    main = new MainComponent(sampleManager);
    main->addActionListener(this);
    addAndMakeVisible(main);
    
    fileBrowse = new FileBrowseComponent(sampleManager);
    fileBrowse->addActionListener(this);
    addAndMakeVisible(fileBrowse);
    
    currentState = grid;
    setMainUI();
    
    setSize (650, 675);
}

SampleBrowserAudioProcessorEditor::~SampleBrowserAudioProcessorEditor()
{
    setLookAndFeel(nullptr);
}

//==============================================================================
void SampleBrowserAudioProcessorEditor::paint (Graphics& g)
{
    
}

void SampleBrowserAudioProcessorEditor::resized()
{
    header->setBounds(0, 0, 650, 50);
    main->setBounds(0, 50, 650, 625);
    fileBrowse->setBounds(0, 50, 650, 625);
}

void SampleBrowserAudioProcessorEditor::setMainUI()
{
    main->setVisible(false);
    fileBrowse->setVisible(false);
    
    switch (currentState)
    {
        case grid:
            main->setVisible(true);
            break;
        case browse:
            fileBrowse->setVisible(true);
            break;
        default:
            break;
    }
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
    else if (message == "file_browse")
    {
        currentState = browse;
        header->setBrowseState();
        setMainUI();
    }
    else if (message == "grid")
    {
        currentState = grid;
        header->setGridState();
        setMainUI();
    }
    else if (message == "add_samples")
    {
        sampleManager->loadNewSamples();
    }
    else
    {
        int pad = message.getIntValue();
        processor.triggerSound(pad);
    }
}
