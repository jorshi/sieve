/*
  ==============================================================================

    SampleGrid.cpp
    Created: 2 Jun 2017 5:53:46pm
    Author:  Jordie Shier 

  ==============================================================================
*/

#include "../JuceLibraryCode/JuceHeader.h"
#include "SampleGrid.h"

//==============================================================================
SampleGrid::SampleGrid(int numSquares, SampleManager* s) : sampleManager(s)
{
    numSquares_ = numSquares;
    Path buttonPath;
    buttonPath.addRoundedRectangle(0, 0, 50, 50, 2);
    
    PadButton* newButton;
    
    int count = 0;
    for (int i = 0; i < numSquares_; ++i)
    {
        for (int j = 0; j < numSquares_; ++j)
        {
            newButton = new PadButton("Pad " + std::to_string(count), padOff, padOver, padDown);
                                      
            newButton->setPadId(count);
            newButton->setShape(buttonPath, true, true, false);
            newButton->setEnabled(false);
            newButton->addActionListener(this);
            addAndMakeVisible(newButton);
            buttonPads.insert(count++, newButton);
        }
    }
}

SampleGrid::~SampleGrid()
{
}

void SampleGrid::paint (Graphics& g)
{
}

void SampleGrid::resized()
{
    float buttonWidth = static_cast<float>(getWidth()) / numSquares_;
    
    int count = 0;
    for (int i = 0; i < numSquares_; ++i)
    {
        for (int j = 0; j < numSquares_; ++j)
        {
            buttonPads.getUnchecked(count)->setBounds((j*buttonWidth)+1, (i*buttonWidth)+1, buttonWidth-2, buttonWidth-2);
            count++;
        }
    }
}

void SampleGrid::actionListenerCallback(const juce::String &message)
{
    sendActionMessage(message);
}


void SampleGrid::updateGrid()
{
    Sample* sample;
    PadButton* pad;
    
    int padCount = numSquares_ * numSquares_;
    for (int i = 0; i < padCount; ++i)
    {
        sample = sampleManager->getSample(i);
        if ((sample = sampleManager->getSample(i)) != nullptr)
        {
            if ((pad = buttonPads.getUnchecked(i)) != nullptr)
            {
                pad->setColours(sample->getColour(),
                                padOver,
                                padDown);
                pad->setEnabled(true);
                pad->setTooltip(sample->getDisplay());
                pad->setSubsetSize(sample->getSubsetSamples());
            }
        } else
        {
            if ((pad = buttonPads.getUnchecked(i)) != nullptr)
            {
                pad->setColours(padOff, padOver, padDown);
                pad->setEnabled(false);
                pad->setTooltip("");
            }
        }
    }
    
    repaint();
}



