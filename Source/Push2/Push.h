/*
  ==============================================================================

    Push.h
    Created: 29 Oct 2019 6:08:58pm
    Author:  Jordie Shier 

  ==============================================================================
*/

#pragma once

#include "JuceToPush2DisplayBridge.h"
#include "../Sample.h"
#include "../SampleType.h"
#include "../CustomLookAndFeel.h"
#include <assert.h>


static const Colour textColour = Colour((uint8)52, (uint8)189, (uint8)235);


class Push
:   public Timer,
    public MidiInputCallback
{
public:
    
    Push();
    ~Push();
    
    NBase::Result Init();
    
    using midiCallbackType = std::function<void(const MidiMessage&)>;
    void SetMidiInputCallback(const midiCallbackType& func);
    
    void addSampleType(const SampleType::Ptr sampleType);
    
    void setPadColours();
    void setPadLED(int padNumber, const Colour& colour);
    void clearPadColours();
    
    void turnControlPadOn(int ccNumber);
    void turnControlPadOff(int ccNumber);
    void clearControlPads();
    
    void resetAllPads();
    
    void incrementSampleTypeSelector();
    void decrementSampleTypeSelector();
    const SampleType::Ptr getSampleType() const;
    
    //------------------------------------------------------------------------------
    void setSample(Sample::Ptr sample);
    
    //------------------------------------------------------------------------------
    
    enum ConnectionStatus { none, displayOnly, midiOnly, connected};
    
    static int mapMidiToPadNumber(const int& midiNumber);
    static int mapPadNumberToMidi(const int& padNumber);
    
private:
    
    NBase::Result openMidiDevice();
    NBase::Result openMidiInputDevice();
    NBase::Result openMidiOutputDevice();
    
    int searchForMidiDevice(const std::string& deviceName);
    
    void handleIncomingMidiMessage (MidiInput *source, const MidiMessage &message) override;
    
    //------------------------------------------------------------------------------
    // Functions for drawing to the Push screen
    void timerCallback() override;
    void drawFrame();
    void drawIntroText();
    
    const SampleType::Ptr getCurrentSampleType() const;
    
    ableton::Push2DisplayBridge bridge_;
    ableton::Push2Display push2Display_;
    std::unique_ptr<MidiInput> midiInput_;
    std::unique_ptr<MidiOutput> midiOutput_;
    midiCallbackType midiCallback_;
    float elapsed_;
    
    std::unique_ptr<Font> pushFont_;
    
    //------------------------------------------------------------------------------
    // Members for the main control display
    Sample::Ptr sample_;
    std::vector<SampleType::Ptr> sampleTypes_;
    int sampleTypeSelector;
    
    
};
