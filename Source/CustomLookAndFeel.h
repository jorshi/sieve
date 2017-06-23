/*
  ==============================================================================

    CustomLookAndFeel.h
    Created: 2 Jun 2017 5:28:39pm
    Author:  Jordie Shier 

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

class CustomLookAndFeel : public LookAndFeel_V4
{
public:
    
    // Default Constructor
    CustomLookAndFeel();
    
    // Custom button drawing
    void drawButtonBackground (Graphics& g, Button& button, const Colour& backgroundColour,
                               bool isMouseOverButton, bool isButtonDown) override;
    
    // Colours
    struct Colours {
        static const ColourGradient background;
        static const ColourGradient backgroundOver;
        static const ColourGradient headerGradient;
        static const Colour buttonEdge;
        static const Colour headerText;
        static const Colour buttons;
        static const Colour buttonOver;
        static const Colour padOver;
        static const Colour padPress;
        static const Colour padOff;
    };
    
    static const Typeface::Ptr offsideTypeface;
    
private:
    
    float buttonRadius;
    
};
