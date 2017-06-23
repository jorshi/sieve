/*
  ==============================================================================

    PadButton.cpp
    Created: 15 Jun 2017 7:30:19pm
    Author:  Jordie Shier 

  ==============================================================================
*/

#include "PadButton.h"


PadButton::PadButton(const String& name, Colour normalColour, Colour colourOver, Colour downColour) :
    ShapeButton::ShapeButton(name, normalColour, colourOver, downColour)
{
    
}

void PadButton::buttonStateChanged()
{
    ShapeButton::buttonStateChanged();
    if (isDown())
    {
        // Send an action
        sendActionMessage(String(padId_));
    }
}
