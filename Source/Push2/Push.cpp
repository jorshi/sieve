/*
  ==============================================================================

    Push.cpp
    Created: 29 Oct 2019 6:08:58pm
    Author:  Jordie Shier 

  ==============================================================================
*/

#include "Push.h"

//------------------------------------------------------------------------------

namespace
{
    bool SMatchSubStringNoCase(const std::string& haystack, const std::string& needle)
    {
        auto it = std::search(
                              haystack.begin(), haystack.end(),
                              needle.begin(), needle.end(),
                              [](char ch1, char ch2)                // case insensitive
                              {
                                  return std::toupper(ch1) == std::toupper(ch2);
                              });
        return it != haystack.end();
    }
}

//------------------------------------------------------------------------------


Push::Push()
{
    
}

Push::~Push()
{
    
}


NBase::Result Push::Init()
{
    // First we initialise the low level push2 object
    NBase::Result result = push2Display_.Init();
    RETURN_IF_FAILED_MESSAGE(result, "Failed to init push2");
    
    // Then we initialise the juce to push bridge
    result = bridge_.Init(push2Display_);
    RETURN_IF_FAILED_MESSAGE(result, "Failed to init bridge");
    
    // Initialises the midi input
    result = openMidiDevice();
    RETURN_IF_FAILED_MESSAGE(result, "Failed to open midi device");
    
    // Reset elapsed time
    elapsed_ = 0;
    
    // Start the timer to draw the animation
    startTimerHz(60);
    
    return NBase::Result::NoError;
}


void Push::SetMidiInputCallback(const midiCallbackType& func)
{
    midiCallback_ = func;
}

void Push::setPadColours()
{
    if (!midiOutput_) return;
    
    for (int i = 36; i < 100; ++i)
    {
        auto message = MidiMessage(144, i, 126);
        midiOutput_->sendMessageNow(message);
    }
}

void Push::clearPadColours()
{
    if (!midiOutput_) return;
    
    for (int i = 36; i < 100; ++i)
    {
        auto message = MidiMessage(144, i, 0);
        midiOutput_->sendMessageNow(message);
    }
}


//------------------------------------------------------------------------------
int Push::mapMidiToPadNumber(const int& midiNumber)
{
    int padNumber = midiNumber - 36;
    int padX = padNumber % 8;
    int padY = (float)padNumber / 8.0;
    padY = (7 - padY) * 8;
    padNumber = padY + padX;
    
    return padNumber;
}

//------------------------------------------------------------------------------




NBase::Result Push::openMidiDevice()
{
    NBase::Result result = openMidiInputDevice();
    if (result.Failed())
    {
        RETURN_IF_FAILED(result);
    }
    
    result = openMidiOutputDevice();
    if (result.Failed())
    {
        RETURN_IF_FAILED(result);
    }
    
    return NBase::Result::NoError;
}


NBase::Result Push::openMidiInputDevice()
{
    const int midiInputDevice = searchForMidiDevice("ableton push 2");
    
    if (midiInputDevice == -1)
        return NBase::Result("Failed to find input midi device for push2");
    
    auto input = MidiInput::openDevice(midiInputDevice, this);
    if (!input)
        return NBase::Result("Failed to open input device");
    
    midiInput_.swap(input);
    midiInput_->start();
    
    return NBase::Result::NoError;
}


NBase::Result Push::openMidiOutputDevice()
{
    const int midiOutputDevice = searchForMidiDevice("ableton push 2");
    
    if (midiOutputDevice == -1)
        return NBase::Result("Failed to find output midi device for push2");
    
    auto output = MidiOutput::openDevice(midiOutputDevice);
    if (!output)
        return NBase::Result("Failed to open output device");
    
    midiOutput_.swap(output);
    
    return NBase::Result::NoError;
}


int Push::searchForMidiDevice(const std::string& deviceName)
{
    // Look for an input device matching push 2
    auto devices = MidiInput::getDevices();
    int deviceIndex = -1;
    int index = 0;
    
    for (auto& device: devices)
    {
        if (SMatchSubStringNoCase(device.toStdString(), deviceName))
        {
            deviceIndex = index;
            break;
        }
        index++;
    }
    
    return deviceIndex;
}


void Push::handleIncomingMidiMessage (MidiInput *source, const MidiMessage &message)
{
    // if a callback has been set, forward the incoming message
    if (midiCallback_)
    {
        midiCallback_(message);
    }
}


void Push::timerCallback()
{
    elapsed_ += 0.02f;
    drawFrame();
}


void Push::drawFrame()
{
    // Request a juce::Graphics from the bridge
    auto& g = bridge_.GetGraphic();
    
    // Clear previous frame
    g.fillAll(juce::Colour(0xff000000));
    
    // Get the height and width of the display
    const auto height = ableton::Push2DisplayBitmap::kHeight;
    const auto width = ableton::Push2DisplayBitmap::kWidth;
    
    // Draw sieve logo
    auto logo = ImageCache::getFromMemory(BinaryData::SieveText_png, BinaryData::SieveText_pngSize);
    g.drawImageAt(logo, (width - logo.getWidth()) / 2 , (height - logo.getHeight()) / 2);
    
    // Tells the bridge we're done with drawing and the frame can be sent to the display
    bridge_.Flip();
}
