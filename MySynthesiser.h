/*
  ==============================================================================

    MySynthesiser.h
    Created: 7 Mar 2020 4:27:57pm
    Author:  Tom Mudd

  ==============================================================================
*/

#pragma once
#include "Oscillator.h"
#include "MultiOscSynth.h"

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
        vol1 = param3;
        vol2 = param4;
        detuneAmount = param5;
        attack = param6;
        decay = param7;
        sustain = param8;
        release = param9;
        cutOff = param10;
        resonance = param11;
        filterType = param12;
        vol3 = param13;
    }

    void initialise(float sampleRate)
    {
        // Oscillators
        oscs.setSampleRate(sampleRate);

        // Envelope
        env.setSampleRate(sampleRate);

        // Smoothed Values;
        smoothedVol1.reset(512);

        sr = sampleRate;
    }

 
    //--------------------------------------------------------------------------
    /**
     What should be done when a note starts

     @param midiNoteNumber
     @param velocity
     @param SynthesiserSound unused variable
     @param / unused variable
     */
    void startNote(int midiNoteNumber, float velocity, SynthesiserSound*, int currentPitchWheelPosition) override
    {
        // Set frequencies
        playing = true;
        freq = MidiMessage::getMidiNoteInHertz(midiNoteNumber);
        pitchWheelMoved(currentPitchWheelPosition);
        oscs.setOscFrequency(1, freq * pitchBend);
        oscs.setOscFrequency(2, freq * pitchBend);
        
        
        // Start envelope
        ending = false;
        env.reset();
        envParams.attack = *attack;
        envParams.decay = *decay;
        envParams.sustain = *sustain;
        envParams.release = *release;
        env.setParameters(envParams);
        env.noteOn();
    }

    // ----------------------------------------------------------------------------
    /**
    Set sample rates for all oscillators
    @param sampleRate
    */


    //---------------------------------------------------------------------------
    /*
    Set frequencies of oscillators
    @osc (1 or 2)
    @freq
    */


    //---------------------------------------------------------------------------
    /*
    Process a specific oscillator deoending on user choice
    @osc (1 or 2)
    */


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
            updateParameters();

            for (int sampleIndex = startSample; sampleIndex < (startSample + numSamples); sampleIndex++)
            {
                // Update Parameters
                updateParameters();

                // Set frequencies
                oscs.setOscFrequency(0, freq * pitchBend);
                oscs.setOscFrequency(1, freq * detune * pitchBend);

                // Process oscs
                float currentSample = 0.5f * (oscs.process(0, (int)*osc1Type) * gain1 + oscs.process(1, (int)*osc2Type) * gain2 + noise.process() * gain3);

                // Get envelope value
                float envelopeValue = env.getNextSample();
                currentSample *= envelopeValue;

                // Write to output channels
                for (int chan = 0; chan < outputBuffer.getNumChannels(); chan++)
                {
                    outputBuffer.addSample(chan, sampleIndex, currentSample);
                }

                // Clear note
                if (ending)
                {
                    if (envelopeValue < 0.001f)
                    {
                        clearCurrentNote();
                        playing = false;
                        
                    }
                }
            }
        }
    }
    //--------------------------------------------------------------------------
    /*
    Update parameters from UI
    */
    void updateParameters()
    {
        smoothedVol1.setTargetValue(*vol1);
        smoothedVol2.setTargetValue(*vol2);
        smoothedVol3.setTargetValue(*vol3);
        gain1 = smoothedVol1.getNextValue();
        gain2 = smoothedVol2.getNextValue();
        gain3 = smoothedVol3.getNextValue();
        detune = 1.0f + 0.5f * pow(*detuneAmount, 3);
    }

    //--------------------------------------------------------------------------
    void pitchWheelMoved(int currentPitchWheelPosition) override {
        float wheelVal = currentPitchWheelPosition / 16386.0f;
        float upper = pow(2, pitchBendNumSemitones * 100 / 1200.0f);
        float lower = 1 / upper;
        pitchBend  = jmap(wheelVal, lower, upper);
    }

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
    
    // General
    float sr;

    // Oscillators
    MultiOscSynth oscs{2};
    float freq;
    float detune = 1.0f;
    std::atomic <float>* osc1Type;
    std::atomic <float>* osc2Type;
    std::atomic <float>* detuneAmount;
    std::atomic <float>* vol1;
    std::atomic <float>* vol2;
    SmoothedValue<float, ValueSmoothingTypes::Linear> smoothedVol1;
    SmoothedValue<float, ValueSmoothingTypes::Linear> smoothedVol2;
    float gain1 = 0.5;
    float gain2 = 0.5;

    /// Noise
    std::atomic <float >* vol3;
    SmoothedValue<float, ValueSmoothingTypes::Linear> smoothedVol3;
    float gain3 = 0.5f;
    Noise noise;

    // Envelope
    bool playing = false;
    bool ending = false;
    ADSR env;
    ADSR::Parameters envParams;
    std::atomic <float>* attack;
    std::atomic <float>* decay;
    std::atomic <float>* sustain;
    std::atomic <float>* release;

    // Filter
    //IIRFilter filter;
    std::atomic <float >* cutOff;
    std::atomic <float >* resonance;
    std::atomic <float >* filterType;

    float pitchBend = 1.0f;
    int pitchBendNumSemitones = 2;

};