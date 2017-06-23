/*
  ==============================================================================

    SampleGrid.h
    Created: 2 Jun 2017 5:53:46pm
    Author:  Jordie Shier 

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "SamplePad.h"
#include "PadButton.h"
#include "SampleManager.h"
#include "Sample.h"
#include "CustomLookAndFeel.h"

//==============================================================================
/*
*/
class SampleGrid    : public Component,
                      public ActionListener,
                      public ActionBroadcaster
{
public:
    SampleGrid(int numSquares, SampleManager* s);
    ~SampleGrid();

    void paint (Graphics&) override;
    void resized() override;
    
    // Update the grid with samples held in the sample manager
    void updateGrid();

private:
    
    // Listens for a button press
    void actionListenerCallback(const String& message) override;
    
    // Number of squares along a row
    int numSquares_;
    
    // Pad colours;
    const Colour padOff = Colours::grey;
    const Colour padOver = Colours::burlywood;
    const Colour padDown = Colours::aqua;
    
    // All the pads
    OwnedArray<PadButton> buttonPads;
    
    // Pointer to the sample manager
    SampleManager* sampleManager;
    
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SampleGrid)
};
