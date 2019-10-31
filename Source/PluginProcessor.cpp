/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"



//==============================================================================
SampleBrowserAudioProcessor::SampleBrowserAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
{
    logger = FileLogger::createDefaultAppLogger("/Users/jshier/Development", "sieve.log", "Sieve Log");
    juce::Logger::setCurrentLogger(logger.get());
    juce::Logger::writeToLog("Starting to Log");
    
    try {
        sampleManager_ = new SampleManager;
    } catch (const std::exception& e) {
        juce::Logger::writeToLog(e.what());
    }
    

    sampler_ = new Synthesiser;
    
    // Make an 8 voice sampler
    for (int i = 0; i < 8; i++)
    {
        sampler_->addVoice(new SamplerVoice);
    }
    
    startNote_ = 48;
    
    for (int i = 0; i < 64; i++) {
        BigInteger* bigInt = new BigInteger();
        bigInt->setRange(startNote_+i, 1, true);
        midiNotes_.add(bigInt);
    }
    

}

SampleBrowserAudioProcessor::~SampleBrowserAudioProcessor()
{
    juce::Logger::setCurrentLogger(nullptr);
    shutdownPush2();
}

//==============================================================================
const String SampleBrowserAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool SampleBrowserAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool SampleBrowserAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

double SampleBrowserAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int SampleBrowserAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int SampleBrowserAudioProcessor::getCurrentProgram()
{
    return 0;
}

void SampleBrowserAudioProcessor::setCurrentProgram (int index)
{
}

const String SampleBrowserAudioProcessor::getProgramName (int index)
{
    return {};
}

void SampleBrowserAudioProcessor::changeProgramName (int index, const String& newName)
{
}

//==============================================================================
void SampleBrowserAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    sampler_->setCurrentPlaybackSampleRate(sampleRate);
}

void SampleBrowserAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool SampleBrowserAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    if (layouts.getMainOutputChannelSet() != AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void SampleBrowserAudioProcessor::processBlock (AudioSampleBuffer& buffer, MidiBuffer& midiMessages)
{
    const int totalNumInputChannels  = getTotalNumInputChannels();
    const int totalNumOutputChannels = getTotalNumOutputChannels();

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (int i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    // Playback samples that have been triggered
    sampler_->renderNextBlock(buffer, midiMessages, 0, buffer.getNumSamples());
}

//==============================================================================
bool SampleBrowserAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

AudioProcessorEditor* SampleBrowserAudioProcessor::createEditor()
{
    connectPush2();
    return new SampleBrowserAudioProcessorEditor (*this);
}

//==============================================================================
void SampleBrowserAudioProcessor::getStateInformation (MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void SampleBrowserAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
void SampleBrowserAudioProcessor::loadSamplerSounds()
{
    Sample::Ptr sample;
    SamplerSound::Ptr sound;
    BigInteger midiNotes;
    AudioFormatReader* reader;
    
    // Clear current sounds
    sampler_->clearSounds();
    sounds_.clear();
    
    // Clear push colours
    if (push_)
    {
        push_->clearPadColours();
        push_->setSample(nullptr);
    }
    
    // TODO -- the number of pads should be some sort of setting variable somewhere
    for (int i = 0; i < 64; i++)
    {
        sample = sampleManager_->getSample(i);
        if (sample != nullptr)
        {
            reader = sampleManager_->getReaderForSample(*sample);
            double length = reader->lengthInSamples / reader->sampleRate;
            midiNotes.setRange(startNote_+i, 1, true);
            
            // Create a new sampler sound for this sound
            sound = new SamplerSound("Pad " + String(i),
                                     *reader,
                                     *midiNotes_.getUnchecked(i),
                                     startNote_+i,
                                     0.001,
                                     length,
                                     length);
            sampler_->addSound(sound);
            delete reader;
            
            // Set pad colour on push interface
            if (push_)
                push_->setPadLED(i, sample->getColour());
        }
    }
}

void SampleBrowserAudioProcessor::triggerSound(int pad)
{
    sampler_->noteOn(1, pad + startNote_, 0.9);
}

//==============================================================================
void SampleBrowserAudioProcessor::connectPush2()
{
    // Check for Push 2
    if (!push_)
        push_.reset(new Push);
    
    auto result = push_->Init();
    
    if (result.Succeeded())
    {
        DBG("Successfully connected to Push");
        push_->SetMidiInputCallback([this](const MidiMessage& message) {
            this->processPushMidi(message);
        });
        
        const auto& sampleTypes = sampleManager_->getSampleTypes();
        for (const auto& type: sampleTypes)
        {
            push_->addSampleType(type.second);
        }
    }
    else
    {
        DBG(result.GetDescription());
    }
}

void SampleBrowserAudioProcessor::shutdownPush2()
{
    if (push_)
    {
        push_->resetAllPads();
    }
    
    push_.reset(nullptr);
}

//==============================================================================
void SampleBrowserAudioProcessor::processPushMidi(const MidiMessage& message)
{
    if (message.isNoteOn())
    {
        const int noteNumber = message.getNoteNumber();
        if (noteNumber >= 36 && noteNumber <= 99)
        {
            int padNumber = Push::mapMidiToPadNumber(noteNumber);
            auto sample = sampleManager_->getSample(padNumber);
            if (sample)
            {
                triggerSound(padNumber);
                sendActionMessage("midi_trigger" + String(padNumber));
                
                zoomInButtonOn(sample->getSubsetSamples());
                
                if (push_)
                    push_->setSample(sample);
            }
        }
    }
    else if (message.isController())
    {
        const int ccNumber = message.getControllerNumber();
        const int ccValue = message.getControllerValue();
        
        DBG("CC: " << ccNumber << " : " << ccValue);
        
        if (ccNumber == 20 && ccValue > 0)
        {
            sendActionMessage("zoomin");
        }
        else if (ccNumber == 21 && ccValue > 0)
        {
            sendActionMessage("zoomout");
        }
        else if (ccNumber == 71)
        {
            if (ccValue == 1)
                push_->incrementSampleTypeSelector();
            else
                push_->decrementSampleTypeSelector();
        }
        else if (ccNumber == 102 && ccValue > 0)
        {
            const SampleType::Ptr type = push_->getSampleType();
            
            const MessageManagerLock mmLock;
            sampleManager_->updateGrid((int)type->getId());
            sendActionMessage("update_grid");
        }
    }
}

void SampleBrowserAudioProcessor::zoomOutButtonOn(bool isOn)
{
    if (push_)
    {
        if (isOn)
            push_->turnControlPadOn(21);
        else
            push_->turnControlPadOff(21);
    }
}

void SampleBrowserAudioProcessor::zoomInButtonOn(bool isOn)
{
    if (push_)
    {
        if (isOn)
            push_->turnControlPadOn(20);
        else
            push_->turnControlPadOff(20);
    }
}

//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new SampleBrowserAudioProcessor();
}
