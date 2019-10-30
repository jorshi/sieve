/*
  ==============================================================================

    Push.h
    Created: 29 Oct 2019 6:08:58pm
    Author:  Jordie Shier 

  ==============================================================================
*/

#pragma once

#include "JuceToPush2DisplayBridge.h"
#include <assert.h>

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
    
    void setPadColours();
    void setPadLED(int padNumber, const Colour& colour);
    void clearPadColours();
    
    void turnControlPadOn(int ccNumber);
    void turnControlPadOff(int ccNumber);
    void clearControlPads();
    
    void resetAllPads();
    
    enum ConnectionStatus { none, displayOnly, midiOnly, connected};
    
    static int mapMidiToPadNumber(const int& midiNumber);
    static int mapPadNumberToMidi(const int& padNumber);
    
private:
    
    NBase::Result openMidiDevice();
    NBase::Result openMidiInputDevice();
    NBase::Result openMidiOutputDevice();
    
    int searchForMidiDevice(const std::string& deviceName);
    
    void handleIncomingMidiMessage (MidiInput *source, const MidiMessage &message) override;
    
    void drawFrame();
    void drawIntroText();
    void timerCallback() override;
    
    ableton::Push2DisplayBridge bridge_;
    ableton::Push2Display push2Display_;
    std::unique_ptr<MidiInput> midiInput_;
    std::unique_ptr<MidiOutput> midiOutput_;
    midiCallbackType midiCallback_;
    float elapsed_;
};
