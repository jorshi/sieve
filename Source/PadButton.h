/*
  ==============================================================================

    PadButton.h
    Created: 15 Jun 2017 7:30:19pm
    Author:  Jordie Shier 

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "Sample.h"

class PadButton : public ShapeButton,
                  public ActionBroadcaster
{
public:
    
    PadButton(const String& name, Colour normalColour, Colour colourOver, Colour downColour);
    
    void setPadId(const int& id) { padId_ = id; };
    void setSubsetSize(const int& n) { subsetSize_ = n; };
    
    // Custom painting function
    void paintButton (Graphics&, bool isMouseOverButton, bool isButtonDown) override;
    void setColours (Colour newNormalColour, Colour newOverColour, Colour newDownColour);
    
private:
    
    // Override to send message on button depressed
    void buttonStateChanged() override;
    
    int subsetSize_;
    int padId_;
    
    Colour padColour_;
};
