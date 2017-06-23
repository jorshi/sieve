/*
  ==============================================================================

    HeaderComponent.h
    Created: 19 Jun 2017 4:34:14pm
    Author:  Jordie Shier 

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "CustomLookAndFeel.h"

//==============================================================================
/*
*/
class HeaderComponent    : public Component
{
public:
    HeaderComponent();
    ~HeaderComponent();

    void paint (Graphics&) override;
    void resized() override;

private:
    
    ScopedPointer<Font> headerFont;
    TextButton sampleManagerButton;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (HeaderComponent)
};
