/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "MySynthesiser.h"
#include "DelayLine.h"
#include "Lfo.h"
#include "MyLadderFilter.h"

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

    void updateFilterParameters();
    void updateOtherParameters();
    void setTargetValues();

private:
    // SYNTH -----------------------------------------------
    Synthesiser synth;
    int voiceCount = 8;

    // FILTER ----------------------------------------------
    LadderFilter filter;
    Lfo filterLfo;
    float filterLfoVal;

    // CHORUS ----------------------------------------------
    Chorus chorus{ 2 };
    float currentChorusDelay = 5.0f;
    float currentChorusSpeed = 3.0f;

    // DELAY -----------------------------------------------
    DelayLine delay;
    int numDelayParams;

    // GAIN
    float gain = 0.5f;

    // UI PARAMETERS --------------------------------------
    AudioProcessorValueTreeState parameters;
    std::atomic <float >* osc1Type;
    std::atomic <float >* osc2Type;
    std::atomic <float >* volumeParam1;
    std::atomic <float >* volumeParam2;
    std::atomic <float >* pulseWidthParam;
    std::atomic <float >* noise;
    std::atomic <float >* detuneParam;
    std::atomic <float >* attackParam;
    std::atomic <float >* decayParam;
    std::atomic <float >* sustainParam;
    std::atomic <float >* releaseParam;
    std::atomic <float >* cutOffParam;
    std::atomic <float >* qParam;
    std::atomic <float >* filterOnParam;
    std::atomic <float >* filterType;
    std::atomic <float >* filterLfoFreqParam;
    std::atomic <float >* filterLfoDepthParam;
    std::atomic <float >* delayTimeParam;
    std::atomic <float >* delayBlendParam;
    std::atomic <float >* delayFbParam;
    std::atomic <float >* chorusAmountParam;
    std::atomic <float >* chorusSpeedParam;
    std::atomic <float >* chorusDelayParam;

    // SMOOTHED PARAMETERS
    OwnedArray<SmoothedValue<float>> delayParamsSmooth;
    SmoothedValue<float> chorusAmountSmooth;
    SmoothedValue<float> chorusDelaySmooth;


 
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SynthesiserAudioProcessor)
};
