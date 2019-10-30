// Copyright (c) 2017 Ableton AG, Berlin
//
// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
// THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.

#include "Push2Demo.h"
#include <cctype>

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

NBase::Result Demo::Init()
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


//------------------------------------------------------------------------------

NBase::Result Demo::openMidiDevice()
{
  // Look for an input device matching push 2

  auto devices = MidiInput::getDevices();
  int deviceIndex = -1;
  int index = 0;
    String deviceId;
  for (auto& device: devices)
  {
    if (SMatchSubStringNoCase(device.toStdString(), "ableton push 2"))
    {
      deviceIndex = index;
        deviceId = device;
      break;
    }
    index++;
  }

  if (deviceIndex == -1)
  {
    return NBase::Result("Failed to find input midi device for push2");
  }

  // Try opening the device
  auto input = MidiInput::openDevice(deviceIndex, this);
  if (!input)
  {
    return NBase::Result("Failed to open input device");
  }
    
  // Store and starts listening to the device
  midiInput_.swap(input);
  midiInput_->start();
    

    
    auto output = MidiOutput::openDevice(deviceIndex);
    midiOutput_.swap(output);
    
    for (int i = 36; i < 100; ++i)
    {
        auto message = MidiMessage(144, i, 0);
        midiOutput_->sendMessageNow(message);
    }
    
    
    auto message2 = MidiMessage(0xF0, 0x00, 0x21, 0x1D, 0x01, 0x01, 0x03, 127, 0x00, 0x00, 0x00, 0x00, 0x7F, 0x01, 0x7E, 0x00, 0xF7);
    midiOutput_->sendMessageNow(message2);
    
    
    
    auto message1 = MidiMessage(144, 99, 127);
    midiOutput_->sendMessageNow(message1);
    
    


    
//    auto message = MidiMessage(144, 99, 127);
//    output->sendMessageNow(message);
//    


  return NBase::Result::NoError;
}


//------------------------------------------------------------------------------

void Demo::SetMidiInputCallback(const midicb_t& callback)
{
  midiCallback_ = callback;
}


//------------------------------------------------------------------------------

void Demo::handleIncomingMidiMessage (MidiInput* /*source*/, const MidiMessage &message)
{
  // if a callback has been set, forward the incoming message
  if (midiCallback_)
  {
    midiCallback_(message);
  }
}


//------------------------------------------------------------------------------

void Demo::timerCallback()
{
  elapsed_ += 0.02f;
  drawFrame();
}


//------------------------------------------------------------------------------

void Demo::drawFrame()
{
  // Request a juce::Graphics from the bridge
  auto& g = bridge_.GetGraphic();

  // Clear previous frame
  g.fillAll(juce::Colour(0xff000000));

  // Create a path for the animated wave
  const auto height = ableton::Push2DisplayBitmap::kHeight;
  const auto width = ableton::Push2DisplayBitmap::kWidth;

  Path wavePath;

  const float waveStep = 10.0f;
  const float waveY = height * 0.44f;
  int i = 0;

  for (float x = waveStep * 0.5f; x < width; x += waveStep)
  {
    const float y1 = waveY + height * 0.10f * std::sin(i * 0.38f + elapsed_);
    const float y2 = waveY + height * 0.20f * std::sin(i * 0.20f + elapsed_ * 2.0f);

    wavePath.addLineSegment(Line<float>(x, y1, x, y2), 2.0f);
    wavePath.addEllipse(x - waveStep * 0.3f, y1 - waveStep * 0.3f, waveStep * 0.6f, waveStep * 0.6f);
    wavePath.addEllipse(x - waveStep * 0.3f, y2 - waveStep * 0.3f, waveStep * 0.6f, waveStep * 0.6f);

    ++i;
  }

  // Draw the path
  g.setColour(juce::Colour::greyLevel(0.5f));
  g.fillPath(wavePath);

  // Blit the logo on top
  auto logo = ImageCache::getFromMemory(BinaryData::PushStartup_png, BinaryData::PushStartup_pngSize);
  g.drawImageAt(logo, (width - logo.getWidth()) / 2 , (height - logo.getHeight()) / 2);

  // Tells the bridge we're done with drawing and the frame can be sent to the display
  bridge_.Flip();
}
