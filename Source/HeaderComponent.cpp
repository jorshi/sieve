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
    
    sampleManagerButton = new TextButton;
    sampleManagerButton->setButtonText("Sample Manager");
    sampleManagerButton->addListener(this);
    addAndMakeVisible(sampleManagerButton);
    
    backToGridButton = new TextButton;
    backToGridButton->setButtonText("Back");
    backToGridButton->addListener(this);
    addAndMakeVisible(backToGridButton);
    
    currentState = grid;
    updateHeaderUI();
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
    sampleManagerButton->setBounds(513, 13, 120, 25);
    backToGridButton->setBounds(588, 13, 45, 25);
    
}

void HeaderComponent::buttonClicked(Button* button)
{
    if (button == sampleManagerButton)
    {
        sendActionMessage("file_browse");
    }
    else if (button == backToGridButton)
    {
        sendActionMessage("grid");
    }
}

void HeaderComponent::updateHeaderUI()
{
    sampleManagerButton->setVisible(false);
    backToGridButton->setVisible(false);
    
    if (currentState == grid)
    {
        sampleManagerButton->setVisible(true);
    }
    else if (currentState == browse)
    {
        backToGridButton->setVisible(true);
    }
}

void HeaderComponent::setGridState()
{
    currentState = grid;
    updateHeaderUI();
}

void HeaderComponent::setBrowseState()
{
    currentState = browse;
    updateHeaderUI();
}
