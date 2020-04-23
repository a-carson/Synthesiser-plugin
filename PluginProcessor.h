/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "MySynthesiser.h"
#include "MoogVCF.h"
#include "DelayLine.h"
#include "Lfo.h"

//#include "juce_dsp/processors/juce_ProcessContext.h"
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

    void process(dsp::ProcessContextReplacing <float> context);
    void updateDspParameters();
    void updateOtherParameters();

private:
    float sr;
    AudioProcessorValueTreeState parameters;

    Synthesiser synth;
    int voiceCount = 8;

    std::atomic <float >* osc1Type;
    std::atomic <float >* osc2Type;


    dsp::LadderFilter<float > lowPassLadder;
    dsp::LadderFilter<float > highPassLadder;

    std::atomic <float >* volumeParam1;
    std::atomic <float >* volumeParam2;
    std::atomic <float >* noise;

    std::atomic <float >* detuneParam;
    std::atomic <float >* attackParam;
    std::atomic <float >* decayParam;
    std::atomic <float >* sustainParam;
    std::atomic <float >* releaseParam;

    std::atomic <float >* cutOffParam;
    std::atomic <float >* qParam;
    std::atomic <float >* filterType;
    std::atomic <float >* filterLfoFreqParam;
    std::atomic <float >* filterLfoDepthParam;
    
    Lfo lfo;
    float lfoVal;

    IIRFilter filter;
    MoogLadderFilterLinear moog{};


    DelayLine delay;
    std::atomic <float >* delayTimeParam;
    std::atomic <float >* delayBlendParam;
    std::atomic <float >* feedbackParam;
    SmoothedValue<float> smoothedDelayTime;
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SynthesiserAudioProcessor)
};
