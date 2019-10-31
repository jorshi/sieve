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
: sampleTypeSelector(0)
{
    pushFont_.reset(new Font(CustomLookAndFeel::offsideTypeface));
}

Push::~Push()
{
    resetAllPads();
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
    
    resetAllPads();
    
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


void Push::addSampleType(const SampleType::Ptr sampleType)
{
    const long long typeId = sampleType->getId();
    if (typeId > ((int)sampleTypes_.size() - 1))
    {
        sampleTypes_.resize(typeId + 1);
    }
    
    sampleTypes_.at(typeId) = sampleType;
}


void Push::setPadLED(int padNumber, const Colour &colour)
{
    int colourEntry = padNumber + 1;
    
    uint8 red = colour.getRed();
    uint8 green = colour.getGreen();
    uint8 blue = colour.getBlue();
    uint8 alpha = colour.getAlpha();
    
    uint8 redLSB = red & 127;
    uint8 redMSB = (red & 128) >> 7;
    
    uint8 greenLSB = green & 127;
    uint8 greenMSB = (green & 128) >> 7;
    
    uint8 blueLSB = blue & 127;
    uint8 blueMSB = (blue & 128) >> 7;
    
    auto colourMessage = MidiMessage(0xF0, 0x00, 0x21, 0x1D, 0x01, 0x01, 0x03, colourEntry, redLSB, redMSB, greenLSB, greenMSB, blueLSB, blueMSB, 0x00, 0x00, 0xF7);
    auto padMessage = MidiMessage(144, mapPadNumberToMidi(padNumber), colourEntry);
    
    if (midiOutput_)
    {
        midiOutput_->sendMessageNow(colourMessage);
        midiOutput_->sendMessageNow(padMessage);
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


void Push::turnControlPadOn(int ccNumber)
{
    auto colourMessage = MidiMessage(0xF0, 0x00, 0x21, 0x1D, 0x01, 0x01, 0x03, 90, 82, 0, 30, 1, 125, 1, 0x00, 0x00, 0xF7);
    auto padMessage = MidiMessage(176, ccNumber, 90);
    
    if (midiOutput_)
    {
        midiOutput_->sendMessageNow(colourMessage);
        midiOutput_->sendMessageNow(padMessage);
    }
}

void Push::turnControlPadOff(int ccNumber)
{
    auto padMessage = MidiMessage(176, ccNumber, 0);
    if (midiOutput_)
        midiOutput_->sendMessageNow(padMessage);
}

void Push::clearControlPads()
{
    for (int i = 3; i < 128; ++i)
    {
        auto padMessage = MidiMessage(176, i, 0);
        if (midiOutput_)
            midiOutput_->sendMessageNow(padMessage);
    }
}


void Push::resetAllPads()
{
    clearPadColours();
    clearControlPads();
}

void Push::incrementSampleTypeSelector()
{
    const int maxValue = (int)sampleTypes_.size() * 25;
    sampleTypeSelector = std::min(sampleTypeSelector + 1, maxValue);
    drawFrame();
}

void Push::decrementSampleTypeSelector()
{
    const int minValue = 0;
    sampleTypeSelector = std::max(sampleTypeSelector - 1, minValue);
    drawFrame();
}

const SampleType::Ptr Push::getSampleType() const
{
    return getCurrentSampleType();
}

//------------------------------------------------------------------------------
void Push::setSample(Sample::Ptr sample)
{
    sample_ = sample;
    drawFrame();
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

int Push::mapPadNumberToMidi(const int &padNumber)
{
    int midiX = padNumber % 8;
    int midiY = (float)padNumber / 8.0;
    midiY = (7 - midiY) * 8;
    
    return midiX + midiY + 36;
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
    elapsed_ += 0.01f;
    
    if (elapsed_ > 3.0f)
    {
        drawFrame();
        turnControlPadOn(102);
        stopTimer();
    }
    else
    {
        drawIntroText();
    }
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
    
    Rectangle<int> thumbBounds(width / 4, 0, (3 * width) / 4, height);
    thumbBounds.reduce(10, 15);
    thumbBounds.setY(20);
    
    if (sample_ != nullptr)
    {
        AudioThumbnail& thumbnail = sample_->getThumbnail();
        if (thumbnail.getNumChannels() == 0)
        {
        }
        else
        {
            g.setColour(sample_->getColour());
            thumbnail.drawChannel(g, thumbBounds, 0.0, thumbnail.getTotalLength(), 0, 1.0f);
            
            // Obtain message manager thread lock before drawing text
            const MessageManagerLock mmLock;
            
            g.setFont(*(pushFont_.get()));
            g.setFont(30);
            g.drawText(sample_->getName(), thumbBounds.getX(), 10, thumbBounds.getWidth(), 150, Justification::topRight);
            
            g.setColour(textColour);
            g.setFont(28);
            
            int numSubsetSamples = sample_->getSubsetSamples();
            if (numSubsetSamples)
            {
                g.drawText(String(numSubsetSamples) + " SAMPLES", 20, height - 60, width/8, 30, Justification::bottomLeft);
                g.drawText("ZOOM IN", 20, height - 30, width/8, 30, Justification::bottomLeft);
            }
            
            if (sample_->getParent())
            {
                g.drawText("ZOOM OUT", width/8, height/2, width/8, height/2, Justification::centredBottom);
            }
        }
    }
    
    // Obtain message manager thread lock before drawing text
    const MessageManagerLock mmLock;
    
    g.setFont(*(pushFont_.get()));
    g.setFont(30);
    g.setColour(textColour);
    g.drawText("LOAD SAMPLES:", 20, 10, width/4, 35, Justification::centredLeft);
    
    const SampleType::Ptr type = getCurrentSampleType();
    if (type)
    {
        g.drawText(type->getName().toUpperCase(), 20, 40, width/4, 35, Justification::centredLeft);
    }

    
    // Tells the bridge we're done with drawing and the frame can be sent to the display
    bridge_.Flip();
}

void Push::drawIntroText()
{
    // Request a juce::Graphics from the bridge
    auto& g = bridge_.GetGraphic();
    
    // Clear previous frame
    g.fillAll(juce::Colour(0xff000000));
    
    // Get the height and width of the display
    const auto height = ableton::Push2DisplayBitmap::kHeight;
    const auto width = ableton::Push2DisplayBitmap::kWidth;
    
    // Draw Logo
    g.setFont(*(pushFont_.get()));
    g.setFont(height - 20);
    Colour fadeInColour = Colour(textColour.getRed(), textColour.getGreen(), textColour.getBlue(), elapsed_ / 3.0f);
    g.setColour(fadeInColour);
    g.drawText("SIEVE", 0, 0, width, height, Justification::centred);
    
    
    // Tells the bridge we're done with drawing and the frame can be sent to the display
    bridge_.Flip();
}


const SampleType::Ptr Push::getCurrentSampleType() const
{
    const size_t numberOfTypes = sampleTypes_.size();
    
    // Return if no types
    if (numberOfTypes < 1) return nullptr;
    
    int index = sampleTypeSelector / 25;
    
    if (index >= 0 && index < numberOfTypes)
        return sampleTypes_[index];

    else if (index < 0)
        return sampleTypes_[0];

    else
        return sampleTypes_[numberOfTypes - 1];
}
