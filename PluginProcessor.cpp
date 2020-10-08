/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
SynthesiserAudioProcessor::SynthesiserAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", AudioChannelSet::stereo(), true)
                     #endif
                       ),
#endif
    parameters(*this, nullptr, "ParamTreeIdentifier", {
    // Osc Types
    std::make_unique < AudioParameterChoice >("osc1_type", "Oscillator 1", StringArray{ "Saw", "Sine", "Square", "Triangle"}, 0),
    std::make_unique < AudioParameterChoice >("osc2_type", "Oscillator 2", StringArray{ "Saw", "Sine", "Square", "Triangle"}, 0),
    std::make_unique < AudioParameterFloat >("vol1", "Osc 1 Mix", 0.000001f, 1.0f , 0.8f) ,
    std::make_unique < AudioParameterFloat >("vol2", "Osc 2 Mix", 0.000001f, 1.0f , 0.8f) ,
    std::make_unique < AudioParameterFloat >("detune", "Detune", -1.0f, 1.0f , 0.0f) ,
    std::make_unique < AudioParameterFloat >("pw", "Pulse Width", 0.0f, 50.0f , 50.0f) ,
    std::make_unique < AudioParameterFloat >("noise", "Noise", 0.0f, 1.0f , 0.0f) ,

    // Filter
    std::make_unique < AudioParameterBool >("filter_bool", "Filter On/Off", 1),
    std::make_unique < AudioParameterChoice >("filter_type", "Filter Type", StringArray{ "LP12", "LP24", "HP12", "HP24"}, 1),
    std::make_unique < AudioParameterFloat >("cut_off", "Filter Cut-off", 0.0f, 1.0f , 0.5f) ,
    std::make_unique < AudioParameterFloat >("q", "Filter Q", 0.0f, 1.0f , 0.75f) ,
    std::make_unique < AudioParameterFloat >("lfo_depth", "Filter LFO Depth", 0.0f, 1.0f, 0.0f),
    std::make_unique < AudioParameterFloat >("lfo_freq", "Filter LFO Rate", 0.00f, 1.0f, 0.5f),

    // Chorus
    std::make_unique < AudioParameterFloat >("chorus_amount", "Chorus Amount", 0.0f, 1.0f, 1.0f),
    std::make_unique < AudioParameterFloat >("chorus_speed", "Speed", 0.1f, 10.0f , 3.0f),
    std::make_unique < AudioParameterFloat >("chorus_delay", "Depth", 1.0f, 15.0f , 5.0f),

    // Delay
    std::make_unique < AudioParameterFloat >("delay_blend", "Delay Amount", 0.0f, 1.0f , 0.0f),
    std::make_unique < AudioParameterFloat >("delay_time", "Delay Time", 10.0f, 2000.0f , 500.0f),
    std::make_unique < AudioParameterFloat >("delay_fb", "Delay Feedback", 0.0f, 1.0f , 0.8f),

    // Env
    std::make_unique < AudioParameterFloat >("attack", "Attack", 0.002f, 3.0f , 0.1f) ,
    std::make_unique < AudioParameterFloat >("decay", "Decay", 0.002f, 3.0f , 0.5f) ,
    std::make_unique < AudioParameterFloat >("sustain", "Sustain", 0.01f, 1.0f , 0.75f) ,
    std::make_unique < AudioParameterFloat >("release", "Release", 0.01f, 5.0f , 0.5f) })


{// Constructor ///////////////////////

    // Synth Voices
    for (int i = 0; i < voiceCount; i++)
    {
        synth.addVoice(new MySynthVoice() );
    }

    synth.addSound(new MySynthSound());

    // Parameters +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    // Oscillators
    osc1Type = parameters.getRawParameterValue("osc1_type");
    osc2Type = parameters.getRawParameterValue("osc2_type");
    volumeParam1 = parameters.getRawParameterValue("vol1");
    volumeParam2 = parameters.getRawParameterValue("vol2");    
    detuneParam = parameters.getRawParameterValue("detune");
    noise = parameters.getRawParameterValue("noise");
    pulseWidthParam = parameters.getRawParameterValue("pw");

    // Envelope
    attackParam = parameters.getRawParameterValue("attack");
    decayParam = parameters.getRawParameterValue("decay");
    sustainParam = parameters.getRawParameterValue("sustain");
    releaseParam = parameters.getRawParameterValue("release");

    // Filter
    filterOnParam = parameters.getRawParameterValue("filter_bool");
    cutOffParam = parameters.getRawParameterValue("cut_off");
    qParam = parameters.getRawParameterValue("q");
    filterType = parameters.getRawParameterValue("filter_type");
    filterLfoFreqParam = parameters.getRawParameterValue("lfo_freq");
    filterLfoDepthParam = parameters.getRawParameterValue("lfo_depth");

    // Delay
    delayTimeParam = parameters.getRawParameterValue("delay_time");
    delayBlendParam = parameters.getRawParameterValue("delay_blend");
    delayFbParam = parameters.getRawParameterValue("delay_fb");
    
    // Chorus
    chorusAmountParam = parameters.getRawParameterValue("chorus_amount");
    chorusSpeedParam = parameters.getRawParameterValue("chorus_speed");
    chorusDelayParam = parameters.getRawParameterValue("chorus_delay");

    
    for (int i = 0; i < 3; i++)
    {
        delayParamsSmooth.add(new SmoothedValue<float>);
    }

    for (int i = 0; i < voiceCount; i++)
    {
        MySynthVoice* v = dynamic_cast<MySynthVoice*>(synth.getVoice(i));
        v->setParameterPointers(osc1Type,
                                osc2Type,
                                volumeParam1,
                                volumeParam2,
                                detuneParam, 
                                attackParam, 
                                decayParam, 
                                sustainParam, 
                                releaseParam, 
                                noise,
                                pulseWidthParam);
    }



}///////////////////////////////////////

SynthesiserAudioProcessor::~SynthesiserAudioProcessor()
{
}

//==============================================================================
const String SynthesiserAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool SynthesiserAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool SynthesiserAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool SynthesiserAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double SynthesiserAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int SynthesiserAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int SynthesiserAudioProcessor::getCurrentProgram()
{
    return 0;
}

void SynthesiserAudioProcessor::setCurrentProgram (int index)
{
}

const String SynthesiserAudioProcessor::getProgramName (int index)
{
    return {};
}

void SynthesiserAudioProcessor::changeProgramName (int index, const String& newName)
{
}

//==============================================================================
void SynthesiserAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    
    // Synth prepare to play ++++++++++++++++++++++++++++++++++++

    synth.setCurrentPlaybackSampleRate(sampleRate);

    for (int i = 0; i < voiceCount; i++)
    {
        MySynthVoice* v = dynamic_cast<MySynthVoice*>(synth.getVoice(i));
        v->initialise(sampleRate);
    }


    // DSP prepare to play +++++++++++++++++++++++++++++++++++++++

    // General
    dsp::ProcessSpec spec;
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = samplesPerBlock;
    spec.numChannels = getTotalNumOutputChannels();

    // Filter
    filter.prepare(spec);
    filterLfo.setSampleRate(sampleRate);
    filterLfo.setFrequency(*filterLfoFreqParam);

    // Delay
    delay.setSampleRate(sampleRate);
    delay.setMaxDelayTime(5000);
    delay.setDelayTime(500);
    delay.setFeedbackAmount(0.5f);

    for (int i = 0; i < 3; i++)
    {
        delayParamsSmooth[i]->reset(sampleRate, 0.5f);
        delayParamsSmooth[i]->setCurrentAndTargetValue(0.0);
    }

    // Chorus
    chorus.initialise(sampleRate, 20.0f);
    chorus.setParameters(5.0f, 3.0f, 1.0f);

    chorusAmountSmooth.reset(sampleRate, 0.1);
    chorusAmountSmooth.setCurrentAndTargetValue(0.0f);
    chorusDelaySmooth.reset(sampleRate, 0.1);
    chorusDelaySmooth.setCurrentAndTargetValue(0.0f);
    chorusDelaySmooth.setTargetValue(*chorusDelayParam);

}

void SynthesiserAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool SynthesiserAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
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

void SynthesiserAudioProcessor::updateFilterParameters()
{
    // Lfo Parameters
    float lfoFreq = 10.0f * pow(2, 10.0f * (*filterLfoFreqParam - 1.0f));
    filterLfo.setFrequency(lfoFreq);
    filterLfo.setDepth(*filterLfoDepthParam);

    // Filter
    float cutOff = *cutOffParam * filterLfoVal;     
    filter.setNormalisedCutOff(cutOff);
    filter.setResonance(*qParam);
}


void SynthesiserAudioProcessor::setTargetValues()
{
    delayParamsSmooth[0]->setTargetValue(*delayTimeParam);
    delayParamsSmooth[1]->setTargetValue(*delayBlendParam);
    delayParamsSmooth[2]->setTargetValue(*delayFbParam);
    chorusAmountSmooth.setTargetValue(*chorusAmountParam);

    // if chorus slider changed...
    if (currentChorusDelay != *chorusDelayParam)
    {
        chorusDelaySmooth.setTargetValue(*chorusDelayParam);
        currentChorusDelay = *chorusDelayParam;
    }
}

void SynthesiserAudioProcessor::updateOtherParameters()
{
    delay.setDelayTime(delayParamsSmooth[0]->getNextValue());
    delay.setFeedbackAmount(delayParamsSmooth[2]->getNextValue());

    // if speed slider changed
    if (currentChorusSpeed != *chorusSpeedParam)
    {
        chorus.setSpeed(*chorusSpeedParam);
        currentChorusSpeed = *chorusSpeedParam;
    }

    // set delay time until smoothed valule reaches target value
    if (chorusDelaySmooth.getNextValue() != currentChorusDelay)
    {
        chorus.setDelayTime(chorusDelaySmooth.getNextValue());
    }
 
}


void SynthesiserAudioProcessor::processBlock (AudioBuffer<float>& buffer, MidiBuffer& midiMessages)
{
    ScopedNoDenormals noDenormals;

    // SYNTH
    synth.renderNextBlock(buffer, midiMessages, 0, buffer.getNumSamples());
    
    // FILTER
    dsp::AudioBlock <float> block (buffer);
    updateFilterParameters();
    if (*filterOnParam)
        filter.process((int)*filterType, block);

    // GET BUFFER
    float* left = buffer.getWritePointer(0);
    float* right = buffer.getWritePointer(1);

    setTargetValues();


    for (int i = 0; i < buffer.getNumSamples(); i++)
    {

        updateOtherParameters();

        filterLfoVal = filterLfo.process();
        
        left[i] += chorusAmountSmooth.getNextValue() * chorus.process(left[i], 0);
        right[i] += chorusAmountSmooth.getNextValue() * chorus.process(right[i], 1);

        left[i] += delayParamsSmooth[1]->getNextValue() * delay.process(left[i]);
        right[i] += delayParamsSmooth[1]->getNextValue() * delay.process(right[i]);

        left[i] *= gain;
        right[i] *= gain;
    }

}

//==============================================================================
bool SynthesiserAudioProcessor::hasEditor() const
{
    return false; // (change this to false if you choose to not supply an editor)
}

AudioProcessorEditor* SynthesiserAudioProcessor::createEditor()
{
    return new SynthesiserAudioProcessorEditor (*this);
}

//==============================================================================
void SynthesiserAudioProcessor::getStateInformation (MemoryBlock& destData)
{
    auto state = parameters.copyState();
    std::unique_ptr<XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

void SynthesiserAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    std::unique_ptr<XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));
    if (xmlState.get() != nullptr)
    {
        if (xmlState->hasTagName(parameters.state.getType()))
        {
            parameters.replaceState(ValueTree::fromXml(*xmlState));
        }
    }
}

//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new SynthesiserAudioProcessor();
}

