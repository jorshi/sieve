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
    zoomInButton->setEnabled(false);
    addAndMakeVisible(zoomInButton);
    
    zoomOutButton = new TextButton;
    zoomOutButton->setButtonText("Zoom Out");
    zoomOutButton->addListener(this);
    zoomOutButton->setEnabled(false);
    addAndMakeVisible(zoomOutButton);
    
    resetButton = new TextButton;
    resetButton->setButtonText("Reset");
    resetButton->addListener(this);
    resetButton->setEnabled(false);
    addAndMakeVisible(resetButton);
    
    typeCombo = new ComboBox("type_combo");
    typeCombo->addItem("Kicks", 1);
    typeCombo->addItem("Snares", 2);
    typeCombo->addItem("Hats", 3);
    typeCombo->addItem("All", 4);
    addAndMakeVisible(typeCombo);
    
    parentSample = nullptr;
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
    selectedSample = s;
    
    if (s->getSubsetSamples() > 0)
    {
        zoomInButton->setEnabled(true);
    }
    else
    {
        zoomInButton->setEnabled(false);
    }
}

void ControlComponent::zoomIn()
{
    if (selectedSample != nullptr && selectedSample->getSubsetSamples() > 0)
    {
        sampleManager->zoom(selectedSample);
        sendActionMessage("update_grid");
        parentSample = selectedSample;
        selectedSample = nullptr;
        zoomInButton->setEnabled(false);
        zoomOutButton->setEnabled(true);
    }
}

void ControlComponent::zoomOut()
{
    if (parentSample != nullptr)
    {
        if (parentSample->getParent() != nullptr)
        {
            sampleManager->zoom(parentSample->getParent());
        }
        else
        {
            sampleManager->zoomOutFull();
            zoomOutButton->setEnabled(false);
            sendActionMessage("zoomed_out_full");
        }
        
        sendActionMessage("update_grid");
        parentSample = parentSample->getParent();
        selectedSample = nullptr;
        zoomInButton->setEnabled(false);
        
    }
}


// Button callback
void ControlComponent::buttonClicked(Button* button)
{
    if (button == newSamplesButton)
    {
        resetState();
        sampleManager->updateGrid(typeCombo->getSelectedId());
        sendActionMessage("update_grid");
        resetButton->setEnabled(true);
    }
    
    if (button == zoomInButton)
    {
        zoomIn();
    }
    
    if (button == zoomOutButton)
    {
        zoomOut();
    }
    
    if (button == resetButton)
    {
        resetState();
        waveformComponent->updateSampleAndDraw(nullptr);
        selectedSample = nullptr;
        parentSample = nullptr;
        sampleManager->clear();
        sendActionMessage("update_grid");
    }
}

void ControlComponent::resetState()
{
    zoomInButton->setEnabled(false);
    zoomOutButton->setEnabled(false);
    resetButton->setEnabled(false);
}
