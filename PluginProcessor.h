/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "MySynthesiser.h"

//==============================================================================
/**
*/
class SynthesiserAudioProcessor  : public AudioProcessor
{
public:
    //==============================================================================
    SynthesiserAudioProcessor();
    ~SynthesiserAudioProcessor();

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (AudioBuffer<float>&, MidiBuffer&) override;

    //==============================================================================
    AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
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

private:
    
    AudioProcessorValueTreeState parameters;

    Synthesiser synth;
    int voiceCount = 16;

    std::atomic <float >* osc1Type;
    std::atomic <float >* osc2Type;



    std::atomic <float >* volumeParam;
    std::atomic <float >* detuneParam;
    std::atomic <float >* attackParam;
    std::atomic <float >* decayParam;
    std::atomic <float >* sustainParam;
    std::atomic <float >* releaseParam;

    std::atomic <float >* cutOffParam;
    std::atomic <float >* qParam;
    std::atomic <float >* filterType;



    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SynthesiserAudioProcessor)
};
