/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "dbConnector.h"
#include "SampleManager.h"
#include "Push2/Push.h"
#include "Push2/Result.h"


//==============================================================================
/**
*/
class SampleBrowserAudioProcessor  : public AudioProcessor, public ActionBroadcaster
{
public:
    //==============================================================================
    SampleBrowserAudioProcessor();
    ~SampleBrowserAudioProcessor();

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (AudioSampleBuffer&, MidiBuffer&) override;

    //==============================================================================
    AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const String getProgramName (int index) override;
    void changeProgramName (int index, const String& newName) override;

    //==============================================================================
    void getStateInformation (MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;
    
    //==============================================================================
    SampleManager* getSampleManager() { return sampleManager_.get(); };
    void loadSamplerSounds();
    void triggerSound(int pad);
    
    //==============================================================================
    void connectPush2();
    void shutdownPush2();
    
    //==============================================================================
    void processPushMidi(const MidiMessage& message);

    
private:
    //==============================================================================
    ScopedPointer<SampleManager> sampleManager_;
    ScopedPointer<Synthesiser> sampler_;
    ReferenceCountedArray<SamplerSound> sounds_;
    OwnedArray<BigInteger> midiNotes_;
    int startNote_;
    
    ScopedPointer<FileLogger> logger;
    
    std::unique_ptr<Push> push_;
    
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SampleBrowserAudioProcessor)
};
