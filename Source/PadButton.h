/*
  ==============================================================================

    PadButton.h
    Created: 15 Jun 2017 7:30:19pm
    Author:  Jordie Shier 

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

class PadButton : public ShapeButton,
                  public ActionBroadcaster
{
public:
    
    PadButton(const String& name, Colour normalColour, Colour colourOver, Colour downColour);
    
    void setPadId(const int& id) { padId_ = id; };
    
    
private:
    
    // Override to send message on button depressed
    void buttonStateChanged() override;
    
    int padId_;
};
