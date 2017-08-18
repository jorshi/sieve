/*
  ==============================================================================

    ControlComponent.cpp
    Created: 20 Jun 2017 2:38:26pm
    Author:  Jordie Shier 

  ==============================================================================
*/

#include "../JuceLibraryCode/JuceHeader.h"
#include "ControlComponent.h"

//==============================================================================
ControlComponent::ControlComponent(SampleManager* s) : sampleManager(s)
{
    waveformComponent = new WaveformComponent();
    addAndMakeVisible(waveformComponent);
    
    newSamplesButton = new TextButton;
    newSamplesButton->setButtonText("Load Samples");
    newSamplesButton->addListener(this);
    addAndMakeVisible(newSamplesButton);
    
    zoomInButton = new TextButton;
    zoomInButton->setButtonText("Zoom In");
    zoomInButton->addListener(this);
    addAndMakeVisible(zoomInButton);
    
    zoomOutButton = new TextButton;
    zoomOutButton->setButtonText("Zoom Out");
    zoomOutButton->addListener(this);
    addAndMakeVisible(zoomOutButton);
    
    resetButton = new TextButton;
    resetButton->setButtonText("Reset");
    resetButton->addListener(this);
    addAndMakeVisible(resetButton);
    
    typeCombo = new ComboBox("type_combo");
    typeCombo->addItem("Kicks", 1);
    typeCombo->addItem("Snares", 2);
    addAndMakeVisible(typeCombo);
}

ControlComponent::~ControlComponent()
{
}

void ControlComponent::paint (Graphics& g)
{
    g.setGradientFill(CustomLookAndFeel::Colours::background);
    g.fillAll();
    
    g.setColour (Colours::grey);
    g.drawRect (getLocalBounds(), 1);   // draw an outline around the component
}

void ControlComponent::resized()
{
    // Setup the waveform viewer
    int width = getWidth() / 2;
    waveformComponent->setBounds(width, 0, width, getHeight());
    
    newSamplesButton->setBounds(138, 10, 100, 25);
    zoomInButton->setBounds(10, 44, 70, 25);
    zoomOutButton->setBounds(90, 44, 70, 25);
    resetButton->setBounds(170, 44, 68, 25);
    
    typeCombo->setBounds(10, 10, 119, 25);
}

void ControlComponent::updateSelectedSample(Sample::Ptr s)
{
    waveformComponent->updateSampleAndDraw(s);
}

// Button callback
void ControlComponent::buttonClicked(Button* button)
{
    if (button == newSamplesButton)
    {
        sampleManager->updateGrid(typeCombo->getSelectedId());
    }
}
