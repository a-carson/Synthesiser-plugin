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
    std::make_unique < AudioParameterFloat >("noise", "Noise", 0.0f, 1.0f , 0.0f) ,


    // Env
    std::make_unique < AudioParameterFloat >("attack", "Attack", 0.002f, 3.0f , 0.1f) ,
    std::make_unique < AudioParameterFloat >("decay", "Decay", 0.002f, 3.0f , 0.5f) ,
    std::make_unique < AudioParameterFloat >("sustain", "Sustain", 0.01f, 1.0f , 0.75f) ,
    std::make_unique < AudioParameterFloat >("release", "Release", 0.01f, 10.0f , 1.0f) ,

    // Filter
    std::make_unique < AudioParameterFloat >("cut_off", "Cut-off", 0.0f, 1.0f , 0.5f) ,
    std::make_unique < AudioParameterFloat >("q", "Q", 0.0f, 1.0f , 0.75f) ,
    std::make_unique < AudioParameterChoice >("filter_type", "Filter Type",
        StringArray{ "Low Pass", "High Pass", "Band Pass"}, 0),
    std::make_unique < AudioParameterFloat >("lfo_freq", "LFO Rate", 0.00f, 1.0f, 0.5f),
    std::make_unique < AudioParameterFloat >("lfo_depth", "LFO Depth", 0.0f, 1.0f, 0.0f),

    // Delay
    std::make_unique < AudioParameterFloat >("delay_time", "Delay Time", 0.0f, 2.0f , 1.0f),
    std::make_unique < AudioParameterFloat >("delay_blend", "Delay Amount", 0.0f, 1.0f , 0.0f),
    std::make_unique < AudioParameterFloat >("delay_fb", "Feedback", 0.0f, 1.0f , 0.8f),


        })

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

    // Envelope
    attackParam = parameters.getRawParameterValue("attack");
    decayParam = parameters.getRawParameterValue("decay");
    sustainParam = parameters.getRawParameterValue("sustain");
    releaseParam = parameters.getRawParameterValue("release");


    // Filter
    cutOffParam = parameters.getRawParameterValue("cut_off");
    qParam = parameters.getRawParameterValue("q");
    filterType = parameters.getRawParameterValue("filter_type");
    filterLfoFreqParam = parameters.getRawParameterValue("lfo_freq");
    filterLfoDepthParam = parameters.getRawParameterValue("lfo_depth");

    // Delay
    delayTimeParam = parameters.getRawParameterValue("delay_time");
    delayBlendParam = parameters.getRawParameterValue("delay_blend");
    feedbackParam = parameters.getRawParameterValue("delay_fb");
    

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
                                cutOffParam,
                                qParam,
                                filterType,
                                noise);
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
    sr = sampleRate;
    
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

    // Ladder Filters (LP & HP)
    lowPassLadder.prepare(spec);
    highPassLadder.prepare(spec);
    lowPassLadder.setMode(dsp::LadderFilter<float>::Mode::LPF24);
    highPassLadder.setMode(dsp::LadderFilter<float>::Mode::HPF24);
    lfo.setSampleRate(sampleRate);
    lfo.setFrequency(*filterLfoFreqParam);

    // Delay
    delay.setSampleRate(sampleRate);
    delay.setMaxDelayTime(5000);
    delay.setDelayTime(500);
    delay.setFeedbackAmount(0.5f);
    smoothedDelayTime.reset(sampleRate, 0.1);
    smoothedDelayTime.setCurrentAndTargetValue(0.0);

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

void SynthesiserAudioProcessor::updateDspParameters()
{
    // Lfo Parameters
    float lfoFreq = 10.0f * pow(2, 10.0f * (*filterLfoFreqParam - 1.0f));
    lfo.setFrequency(lfoFreq);
    lfo.setDepth(*filterLfoDepthParam);

    // Ladder Filters
    float cutOff = *cutOffParam * lfoVal;           // deference pointer
    cutOff = pow(2, jmap(cutOff, 4.32f, 14.32f));   // convert to frequency
    lowPassLadder.setCutoffFrequencyHz(cutOff);     // set frequency
    lowPassLadder.setResonance(*qParam);            // set resonance    
    highPassLadder.setCutoffFrequencyHz(cutOff);   
    highPassLadder.setResonance(*qParam);
}

void SynthesiserAudioProcessor::updateOtherParameters()
{
    float delayTime = smoothedDelayTime.getNextValue();
    delay.setDelayTime(delayTime);
    delay.setFeedbackAmount(*feedbackParam);
}

void SynthesiserAudioProcessor::process(dsp::ProcessContextReplacing <float> context)
{
    // Ladder Filters
    if (*filterType < 0.5)
        lowPassLadder.process(context);
    if (*filterType > 0.5)
        highPassLadder.process(context);


}



void SynthesiserAudioProcessor::processBlock (AudioBuffer<float>& buffer, MidiBuffer& midiMessages)
{
    ScopedNoDenormals noDenormals;

    // SYNTH
    synth.renderNextBlock(buffer, midiMessages, 0, buffer.getNumSamples());
    
    // DSP
    dsp::AudioBlock <float> block (buffer);
    updateDspParameters();
    process(dsp::ProcessContextReplacing<float>(block));

    // OTHER
    float* left = buffer.getWritePointer(0);
    float* right = buffer.getWritePointer(1);

    smoothedDelayTime.setTargetValue(*delayTimeParam * 1000);

    for (int i = 0; i < buffer.getNumSamples(); i++)
    {
        updateOtherParameters();
        lfoVal = lfo.process();
        left[i] += *delayBlendParam * delay.process(left[i]);
        right[i] += *delayBlendParam * delay.process(right[i]);
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
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void SynthesiserAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new SynthesiserAudioProcessor();
}

