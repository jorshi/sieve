/*
  ==============================================================================

    WaveformComponent.cpp
    Created: 16 Jun 2017 12:36:19am
    Author:  Jordie Shier 

  ==============================================================================
*/

#include "../JuceLibraryCode/JuceHeader.h"
#include "WaveformComponent.h"

//==============================================================================
WaveformComponent::WaveformComponent()
{
    setLookAndFeel(&lookAndFeel);
    sample = nullptr;

}

WaveformComponent::~WaveformComponent()
{
}

void WaveformComponent::paint (Graphics& g)
{
    //g.setGradientFill(CustomLookAndFeel::Colours::background);
    //g.fillAll();
    
    Rectangle<int> thumbBounds = getLocalBounds();
    thumbBounds.reduce(10, 15);
    thumbBounds.setY(20);
    
    g.setGradientFill(CustomLookAndFeel::Colours::background);
    g.fillAll();
    
    if (sample != nullptr)
    {
        AudioThumbnail& thumbnail = sample->getThumbnail();
        if (thumbnail.getNumChannels() == 0)
        {
            g.setColour(CustomLookAndFeel::Colours::headerText);
            g.drawText("No Sample Loaded", 0, (getHeight() / 2) - 5, getWidth(), 10, Justification::centred);
        }
        else
        {
            g.setColour(sample->getColour());
            thumbnail.drawChannel(g, thumbBounds, 0.0, thumbnail.getTotalLength(), 0, 1.0f);
            
            g.setColour(CustomLookAndFeel::Colours::headerText);
            g.drawText(sample->getName(), 0, 5, getWidth(), 10, Justification::centred);
        }
    }
    else
    {
        g.setColour(CustomLookAndFeel::Colours::headerText);
        g.drawText("No Sample Loaded", 0, (getHeight() / 2) - 5, getWidth(), 10, Justification::centred);
    }
    
    g.setColour (Colours::grey);
    g.drawRect (getLocalBounds(), 1);   // draw an outline around the component
}

void WaveformComponent::resized()
{

}

void WaveformComponent::updateSampleAndDraw(Sample::Ptr s)
{
    sample = s;
    repaint();
}
