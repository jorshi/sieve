/*
  ==============================================================================

    PadButton.cpp
    Created: 15 Jun 2017 7:30:19pm
    Author:  Jordie Shier 

  ==============================================================================
*/

#include "PadButton.h"


PadButton::PadButton(const String& name, Colour normalColour, Colour colourOver, Colour downColour) :
    ShapeButton(name, normalColour, colourOver, downColour),
    subsetSize_(0),
    padColour_(normalColour)
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

void PadButton::paintButton(Graphics& g, bool isMouseOverButton, bool isButtonDown)
{
    ShapeButton::paintButton(g, isMouseOverButton, isButtonDown);
    
    // Make a visible note of the subset size
    if (subsetSize_ > 1) {
        float saturation = padColour_.getSaturation() > 0.5 ? 0 : 1;
        Colour textColour = Colour::fromHSV(padColour_.getHue(), saturation, padColour_.getBrightness(), 0.5);
        g.setColour(textColour);
        g.drawText(String(subsetSize_), 0, 0, getWidth(), getHeight(), Justification::centred);
    }
}


void PadButton::setColours (Colour newNormalColour, Colour newOverColour, Colour newDownColour)
{
    ShapeButton::setColours(newNormalColour, newOverColour, newDownColour);
    padColour_ = newNormalColour;
}

