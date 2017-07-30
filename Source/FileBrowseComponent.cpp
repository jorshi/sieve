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
FileBrowseComponent::FileBrowseComponent(SampleManager* m) : manager(m)
{
    fileBrowser = new TextButton;
    fileBrowser->setButtonText("Add Audio Samples");
    fileBrowser->addListener(this);
    addAndMakeVisible(fileBrowser);
    
    startTimer(1000);
}

FileBrowseComponent::~FileBrowseComponent()
{
    stopTimer();
}

void FileBrowseComponent::paint (Graphics& g)
{
    g.setGradientFill(CustomLookAndFeel::Colours::background);
    g.fillAll();
    
    g.setColour(CustomLookAndFeel::Colours::tableHeader);
    g.fillRect(34, 50, 582, 30);
    
    g.setColour(CustomLookAndFeel::Colours::headerText);
    g.drawText("Folder Name", 39, 50, 400, 30, Justification::centredLeft);
    g.drawText("Status", 480, 50, 100, 30, Justification::centredLeft);
 
    // Draw the table entries with alternating colours
    ReferenceCountedArray<SampleFolder>& folders = manager->getSampleFolders();
    for (int i = 0; i < folders.size(); i++)
    {
        if (i % 2 == 0)
        {
            g.setGradientFill(CustomLookAndFeel::Colours::background);
        }
        else
        {
            g.setColour(CustomLookAndFeel::Colours::darkerTable);
        }
        
        g.fillRect(34, (i*40) + 80, 582, 40);
        
        g.setColour(CustomLookAndFeel::Colours::headerText);
        g.drawText(folders.getUnchecked(i)->getFile().getFileName(), 39, 80 + (i*40), 400, 39, Justification::centredLeft);
        g.drawText(folders.getUnchecked(i)->getStatusStr(), 480, 80 + (i*40), 100, 39, Justification::centredLeft);
    }
    
    g.setColour(Colours::grey);
    g.drawRect(34, 50, 582, 541);
    g.drawRect(34, 50, 582, 30);
}

void FileBrowseComponent::resized()
{
    fileBrowser->setBounds(34, 15, 150, 25);
}

//==============================================================================

void FileBrowseComponent::buttonClicked(juce::Button *button)
{
    if (button == fileBrowser)
    {
        manager->loadNewSamples();
        repaint();
    }
}

void FileBrowseComponent::timerCallback()
{
    repaint();
}
