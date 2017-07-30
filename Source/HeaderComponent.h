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
class HeaderComponent    : public Component,
                           public ButtonListener,
                           public ActionBroadcaster
{
public:
    HeaderComponent();
    ~HeaderComponent();

    void paint (Graphics&) override;
    void resized() override;
    
    void setGridState();
    void setBrowseState();

private:
    
    // Button callback
    void buttonClicked(Button* button) override;
    
    void updateHeaderUI();
    
    enum HeaderState {grid, browse};
    HeaderState currentState;
    
    ScopedPointer<Font> headerFont;
    ScopedPointer<TextButton> sampleManagerButton;
    ScopedPointer<TextButton> backToGridButton;
    
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (HeaderComponent)
};
