/*
  ==============================================================================

    FileBrowseComponent.cpp
    Created: 29 Jul 2017 5:29:39pm
    Author:  Jordie Shier 

  ==============================================================================
*/

#include "../JuceLibraryCode/JuceHeader.h"
#include "FileBrowseComponent.h"

//==============================================================================
FileBrowseComponent::FileBrowseComponent()
{
    fileBrowser = new TextButton;
    fileBrowser->setButtonText("Add Audio Samples");
    fileBrowser->addListener(this);
    addAndMakeVisible(fileBrowser);
}

FileBrowseComponent::~FileBrowseComponent()
{
}

void FileBrowseComponent::paint (Graphics& g)
{
    g.setGradientFill(CustomLookAndFeel::Colours::background);
    g.fillAll();
}

void FileBrowseComponent::resized()
{
    fileBrowser->setBounds(77, 15, 150, 25);
}

//==============================================================================

void FileBrowseComponent::buttonClicked(juce::Button *button)
{
    if (button == fileBrowser)
    {
        sendActionMessage("add_samples");
    }
}
