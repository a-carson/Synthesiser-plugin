/*
  ==============================================================================

    MySynthesiser.h
    Created: 7 Mar 2020 4:27:57pm
    Author:  Tom Mudd

  ==============================================================================
*/

#pragma once
#include "Oscillator.h"

// ===========================
// ===========================
// SOUND
class MySynthSound : public SynthesiserSound
{
public:
    bool appliesToNote(int) override { return true; }
    //--------------------------------------------------------------------------
    bool appliesToChannel(int) override { return true; }
};




// =================================
// =================================
// Synthesiser Voice - your synth code goes in here

/*!
 @class MySynthVoice
 @abstract struct defining the DSP associated with a specific voice.
 @discussion multiple MySynthVoice objects will be created by the Synthesiser so that it can be played polyphicially

 @namespace none
 @updated 2019-06-18
 */
class MySynthVoice : public SynthesiserVoice
{
public:
    MySynthVoice() {}

    void setParameterPointers(std::atomic <float>* param1 = nullptr,
                              std::atomic <float>* param2 = nullptr,
                              std::atomic <float>* param3 = nullptr,
                              std::atomic <float>* param4 = nullptr,
                              std::atomic <float>* param5 = nullptr,
                              std::atomic <float>* param6 = nullptr,
                              std::atomic <float>* param7 = nullptr,
                              std::atomic <float>* param8 = nullptr,
                              std::atomic <float>* param9 = nullptr,
                              std::atomic <float>* param10 = nullptr,
                              std::atomic <float>* param11 = nullptr,
                              std::atomic <float>* param12 = nullptr,
                              std::atomic <float>* param13 = nullptr,
                              std::atomic <float>* param14 = nullptr,
                              std::atomic <float>* param15 = nullptr)
    {
        osc1Type = param1;
        osc2Type = param2;
        detuneAmount = param3;
        attack = param4;
        decay = param5;
        sustain = param6;
        release = param7;
        volume = param8;
        cutOff = param9;
        resonance = param10;
        filterType = param11;

    }

    void initialise(float sampleRate)
    {
        // Oscillators
        detuneOsc.setSampleRate(sampleRate);
        setOscSampleRates(sampleRate);
        // Envelope
        env.setSampleRate(sampleRate);

        // Smoothed Values;
        smoothedVolume.reset(1024);
        sr = sampleRate;

        filter.setCoefficients(IIRCoefficients::makeLowPass(sampleRate, 1000.0f));
    }

 
    //--------------------------------------------------------------------------
    /**
     What should be done when a note starts

     @param midiNoteNumber
     @param velocity
     @param SynthesiserSound unused variable
     @param / unused variable
     */
    void startNote(int midiNoteNumber, float velocity, SynthesiserSound*, int /*currentPitchWheelPosition*/) override
    {
        // Freq
        playing = true;
        freq = MidiMessage::getMidiNoteInHertz(midiNoteNumber);
        setOscFrequency(1, freq);
        setOscFrequency(2, freq);
        
        // Env
        ending = false;
        env.reset();
        envParams.attack = *attack;
        envParams.decay = *decay;
        envParams.sustain = *sustain;
        envParams.release = *release;
        env.setParameters(envParams);
        env.noteOn();
    }

   // Set sample rates of oscillators
    void setOscSampleRates(float sampleRate)
    {
        sawOsc1.setSampleRate(sampleRate);
        sineOsc1.setSampleRate(sampleRate);
        squareOsc1.setSampleRate(sampleRate);
        triOsc1.setSampleRate(sampleRate);
        sawOsc2.setSampleRate(sampleRate);
        sineOsc2.setSampleRate(sampleRate);
        squareOsc2.setSampleRate(sampleRate);
        triOsc2.setSampleRate(sampleRate);
    }

    // Set frequencies of oscillators
    void setOscFrequency(int osc, float freq)
    {
        if (osc == 1)
        {
            sawOsc1.setFrequency(freq);
            sineOsc1.setFrequency(freq);
            squareOsc1.setFrequency(freq);
            triOsc1.setFrequency(freq);
        }
            
        if (osc == 2)
        {
            sawOsc2.setFrequency(freq);
            sineOsc2.setFrequency(freq);
            squareOsc2.setFrequency(freq);
            triOsc2.setFrequency(freq);
        }
    }

    // play specific oscillator depending on input + drop down value
    float oscProcess(int osc)
    {
        int oscType;

        if (osc == 1)
        {
            oscType = (int) *osc1Type;

            if (oscType == 0)
                return sawOsc1.process() * 0.5;
            if (oscType == 1)
                return sineOsc1.process();
            if (oscType == 2)
                return squareOsc1.process() * 0.5;
            if (oscType == 3)
                return triOsc1.process();
        }

        if (osc == 2)
        {
            oscType = (int) *osc2Type;

            if (oscType == 0)
                return sawOsc2.process() * 0.5;
            if (oscType == 1)
                return sineOsc2.process();
            if (oscType == 2)
                return squareOsc2.process() * 0.5;
            if (oscType == 3)
                return triOsc2.process();
        }
    }

    // set filter parameters depending on drop-down type
    void setFilterParameters(float freq, float q)
    {
        int type = (int) *filterType;

        if (type == 0)
            filter.setCoefficients(IIRCoefficients::makeLowPass(sr, pow(2, jmap(freq, 0.0f, 1.0f, 4.32f, 14.32f)), q));
        if (type == 1)
            filter.setCoefficients(IIRCoefficients::makeHighPass(sr, pow(2, jmap(freq, 0.0f, 1.0f, 4.32f, 14.32f)), q));
        if (type == 2)
            filter.setCoefficients(IIRCoefficients::makeBandPass(sr, pow(2, jmap(freq, 0.0f, 1.0f, 4.32f, 14.32f)), q));
    }
    //--------------------------------------------------------------------------
    /// Called when a MIDI noteOff message is received
    /**
     What should be done when a note stops

     @param / unused variable
     @param allowTailOff bool to decie if the should be any volume decay
     */
    void stopNote(float /*velocity*/, bool allowTailOff) override
    {
        env.noteOff();
        ending = true;
    }

    //--------------------------------------------------------------------------
    /**
     The Main DSP Block: Put your DSP code in here

     If the sound that the voice is playing finishes during the course of this rendered block, it must call clearCurrentNote(), to tell the synthesiser that it has finished

     @param outputBuffer pointer to output
     @param startSample position of first sample in buffer
     @param numSamples number of smaples in output buffer
     */
    void renderNextBlock(AudioSampleBuffer& outputBuffer, int startSample, int numSamples) override
    {
        if (playing) // check to see if this voice should be playing
        {
            setFilterParameters(*cutOff, *resonance);
            setOscFrequency(2, freq * (1.0f - *detuneAmount));
            smoothedVolume.setTargetValue(*volume);


            // iterate through the necessary number of samples (from startSample up to startSample + numSamples)
            for (int sampleIndex = startSample; sampleIndex < (startSample + numSamples); sampleIndex++)
            {

                float envVal = env.getNextSample();
                float currentSample = (oscProcess(1) * 0.5 + oscProcess(2) * 0.5 )* envVal * 0.5;
                currentSample = filter.processSingleSampleRaw(currentSample);


                // for each channel, write the currentSample float to the output
                for (int chan = 0; chan < outputBuffer.getNumChannels(); chan++)
                {
                    // The output sample is scaled by 0.2 so that it is not too loud by default
                    outputBuffer.addSample(chan, sampleIndex, currentSample);
                }

                if (ending)
                {
                    if (envVal < 0.001f)
                    {
                        clearCurrentNote();
                        playing = false;
                    }
                }
            }
        }
    }
    //--------------------------------------------------------------------------
    void pitchWheelMoved(int) override {}
    //--------------------------------------------------------------------------
    void controllerMoved(int, int) override {}
    //--------------------------------------------------------------------------
    /**
     Can this voice play a sound. I wouldn't worry about this for the time being

     @param sound a juce::SynthesiserSound* base class pointer
     @return sound cast as a pointer to an instance of MySynthSound
     */
    bool canPlaySound(SynthesiserSound* sound) override
    {
        return dynamic_cast<MySynthSound*> (sound) != nullptr;
    }
    //--------------------------------------------------------------------------
private:
    //--------------------------------------------------------------------------
    // Set up any necessary variables here
    /// Should the voice be playing?
    bool playing = false;
    bool ending = false;
    float sr;

    TriOsc triOsc1;
    SineOsc sineOsc1;
    SquareOsc squareOsc1;
    Phasor sawOsc1;
    float osc1Freq;

    TriOsc triOsc2;
    SineOsc sineOsc2;
    SquareOsc squareOsc2;
    Phasor sawOsc2;

    TriOsc detuneOsc;

    std::atomic <float>* osc1Type;
    std::atomic <float>* osc2Type;

    float freq;
    std::atomic <float>* detuneAmount;

    ADSR env;
    ADSR::Parameters envParams;
    std::atomic <float>* attack;
    std::atomic <float>* decay;
    std::atomic <float>* sustain;
    std::atomic <float>* release;

    std::atomic <float>* volume;
    SmoothedValue<float, ValueSmoothingTypes::Linear> smoothedVolume;


    /// a random object for use in our test noise function
    Random random;

    IIRFilter filter;
    std::atomic <float >* cutOff;
    std::atomic <float >* resonance;
    std::atomic <float >* filterType;
};