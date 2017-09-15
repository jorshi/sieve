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
    
    fileViewPort = new Viewport("FileViewPort");
    
    fileList = new FileList(manager);
    addAndMakeVisible(fileList);
    
    fileViewPort->setViewedComponent(fileList, false);
    addAndMakeVisible(fileViewPort);
    
    fileViewPort->setScrollBarsShown(true, false);
    
    //addAndMakeVisible(fileList);
    
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
 
    g.setColour(Colours::grey);
    g.drawRect(34, 50, 582, 541);
    g.drawRect(34, 50, 582, 30);
}

void FileBrowseComponent::resized()
{
    currentFileListSize = fileList->getRequiredHeight();
    fileBrowser->setBounds(34, 15, 150, 25);
    fileViewPort->setBounds(35, 51, 580, 539);
    fileList->setBounds(35, 51, 580, currentFileListSize);
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
    resizeFileList();
    repaint();
}


void FileBrowseComponent::resizeFileList()
{
    if (currentFileListSize != fileList->getRequiredHeight())
    {
        currentFileListSize = fileList->getRequiredHeight();
        fileList->setBounds(35, 51, 580, currentFileListSize);
    }
}
