/*
  ==============================================================================

    HeaderComponent.cpp
    Created: 19 Jun 2017 4:34:14pm
    Author:  Jordie Shier 

  ==============================================================================
*/

#include "../JuceLibraryCode/JuceHeader.h"
#include "HeaderComponent.h"

//==============================================================================
HeaderComponent::HeaderComponent()
{
    headerFont = new Font(CustomLookAndFeel::offsideTypeface);
    headerFont->setHeight(46);
    
    addAndMakeVisible(&sampleManagerButton);
    sampleManagerButton.setButtonText("Sample Manager");
}

HeaderComponent::~HeaderComponent()
{
}

void HeaderComponent::paint (Graphics& g)
{
    g.setGradientFill(CustomLookAndFeel::Colours::headerGradient);
    g.fillAll();
    
    g.setColour(CustomLookAndFeel::Colours::headerText);
    g.setFont(*headerFont);
    g.drawText("SIEVE", 0, 0, 120, 46, Justification::centred);
}

void HeaderComponent::resized()
{
    sampleManagerButton.setBounds(513, 13, 120, 25);

}
