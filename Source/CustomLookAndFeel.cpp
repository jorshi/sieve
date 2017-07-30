/*
  ==============================================================================

    CustomLookAndFeel.cpp
    Created: 2 Jun 2017 5:28:39pm
    Author:  Jordie Shier 

  ==============================================================================
*/

#include "CustomLookAndFeel.h"


CustomLookAndFeel::CustomLookAndFeel()
{
    buttonRadius = 7.0f;
    setColour(TextButton::textColourOffId, Colours::headerText);
    setColour(ComboBox::backgroundColourId, juce::Colours::transparentWhite);
    setColour(ComboBox::textColourId, Colours::headerText);
}

void CustomLookAndFeel::drawButtonBackground (Graphics& g, Button& button, const Colour& backgroundColour,
                                            bool isMouseOverButton, bool isButtonDown)
{
    Rectangle<float> buttonArea = button.getLocalBounds().toFloat();
    g.setColour(Colours::headerText);
    g.drawRoundedRectangle(buttonArea, 0.5f, 1.0f);
    
    if (button.isOver() || button.isDown())
    {
        g.setColour(Colours::buttonOver);
        g.fillRect(buttonArea);
    }
}


// Colours
const Colour CustomLookAndFeel::Colours::buttonEdge = Colour::fromRGB(174, 191, 215);
const Colour CustomLookAndFeel::Colours::headerText = Colour::fromRGB(95, 95, 95);
const Colour CustomLookAndFeel::Colours::buttons = Colour::fromRGB(50, 50, 50);
const Colour CustomLookAndFeel::Colours::buttonOver = Colour::fromRGB(175, 175, 175);
const Colour CustomLookAndFeel::Colours::padOver = juce::Colours::burlywood;
const Colour CustomLookAndFeel::Colours::padPress = juce::Colours::aqua;
const Colour CustomLookAndFeel::Colours::padOff = juce::Colours::grey;
const Colour CustomLookAndFeel::Colours::tableHeader = Colour::fromRGB(160, 160, 160);
const Colour CustomLookAndFeel::Colours::darkerTable = Colour::fromRGB(190, 190, 190);

// Gradients
const ColourGradient CustomLookAndFeel::Colours::background = ColourGradient(
                                                                             Colour::fromRGB(199, 199, 199) ,
                                                                             0.0f,
                                                                             0.0f,
                                                                             Colour::fromRGB(214, 214, 214),
                                                                             0.0f,
                                                                             650.0f,
                                                                             false);
const ColourGradient CustomLookAndFeel::Colours::backgroundOver = ColourGradient(
                                                                             Colour::fromRGB(150, 150, 150) ,
                                                                             0.0f,
                                                                             0.0f,
                                                                             Colour::fromRGB(170, 170, 170),
                                                                             0.0f,
                                                                             650.0f,
                                                                             false);

const ColourGradient CustomLookAndFeel::Colours::headerGradient = ColourGradient(Colour::fromRGB(160, 160, 160),
                                                                                 0.0f,
                                                                                 0.0f,
                                                                                 Colour::fromRGB(146, 146, 146),
                                                                                 0.0f,
                                                                                 50.0f,
                                                                                 false);

// Custom Typeface
const Typeface::Ptr CustomLookAndFeel::offsideTypeface = Typeface::createSystemTypefaceFor(BinaryData::OffsideRegular_ttf, BinaryData::OffsideRegular_ttfSize);
                                                                            
