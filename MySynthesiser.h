/*
  ==============================================================================

    MySynthesiser.h
    Created: 7 Mar 2020 4:27:57pm
    Author:  Tom Mudd

  ==============================================================================
*/

#pragma once
#include "Osc.h"
#include "MultiOscSynth.h"
#include "Lfo.h"

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
        vol3 = param10;
        pw = param11;
    }

    void initialise(float sampleRate)
    {
        // Oscillators
        oscs.setSampleRate(sampleRate);

        // Envelope
        env.setSampleRate(sampleRate);

        // Smoothed Values;
        smoothedVol1.reset(sampleRate, 0.1);
        smoothedVol2.reset(sampleRate, 0.1);
        smoothedVol1.setCurrentAndTargetValue(0.0f);
        smoothedVol2.setCurrentAndTargetValue(0.0f);
        smoothedPw.reset(sampleRate, 0.1);
        smoothedPw.setCurrentAndTargetValue(0.0f);
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
        oscs.setOscFrequency(1, (int)*osc1Type, freq * pitchBend);
        oscs.setOscFrequency(2, (int)*osc2Type, freq * pitchBend);
        
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
            setTargetValues();

            for (int sampleIndex = startSample; sampleIndex < (startSample + numSamples); sampleIndex++)
            {
                // update parameters
                updateParameters();

                // set frequencies
                oscs.setOscFrequency(0, (int)*osc1Type, freq * pitchBend);
                oscs.setOscFrequency(1, (int)*osc2Type, freq * detune * pitchBend);

                // set pulse width of square oscs
                for (int i = 0; i < numOscs; i++)
                {
                    oscs.setPulseWidth(i, smoothedPw.getNextValue() / 100.0f);
                }
                
                // process oscs
                float currentSample = 0.5f * (oscs.process(0, (int)*osc1Type) * gain1 + 
                                              oscs.process(1, (int)*osc2Type) * gain2 + 
                                              noise.process() * gain3);

                currentSample *= env.getNextSample();   //apply envelope

                // Write to output channels
                for (int chan = 0; chan < outputBuffer.getNumChannels(); chan++)
                {
                    outputBuffer.addSample(chan, sampleIndex, currentSample);
                }
            }
        }
    }

    //Set targets for smoothed parameters
    void setTargetValues()
    {
        smoothedVol1.setTargetValue(*vol1);
        smoothedVol2.setTargetValue(*vol2);
        smoothedVol3.setTargetValue(*vol3);
        smoothedPw.setTargetValue(*pw);
    }

    // Updates parameters from UI and smoothed params
    void updateParameters()
    {
        gain1 = smoothedVol1.getNextValue();
        gain2 = smoothedVol2.getNextValue();
        gain3 = smoothedVol3.getNextValue();
        detune = 1.0f + 0.5f * pow(*detuneAmount, 3);
    }

    // Handles pitch wheel moved events and sets pitch bend value
    void pitchWheelMoved(int currentPitchWheelPosition) override {
        float wheelVal = currentPitchWheelPosition / 16386.0f;
        float upper = pow(2, pitchBendNumSemitones * 100 / 1200.0f);
        float lower = 1 / upper;
        pitchBend = jmap(wheelVal, lower, upper);
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
    
    // OSCILLATOR ------------------------------------------------
    const int numOscs = 2;                                              // number of oscillators
    MultiOscSynth oscs{numOscs};                                        // oscillator control class
    float freq;                                                         // current note frequency
    float detune = 1.0f;                                                // scaled tuning for osc 2
    float gain1 = 0.5;                                                  // osc 1 gain (current)
    float gain2 = 0.5;                                                  // osc 2 gain (current)
    float pitchBend = 1.0f;                                             // current pitch bend value
    int pitchBendNumSemitones = 2;                                      // controls the extent of the pitch wheel effect
    std::atomic <float>* osc1Type;                                      // osc 1 type
    std::atomic <float>* osc2Type;                                      // osc 2 type
    std::atomic <float>* detuneAmount;                                  // raw detune parameter from UI
    std::atomic <float>* vol1;                                          // osc 1 gain (raw)
    std::atomic <float>* vol2;                                          // osc 2 gain (raw)
    std::atomic <float>* pw;                                            // square wave pulse width (raw)
    SmoothedValue<float, ValueSmoothingTypes::Linear> smoothedVol1;     // osc 1 gain (smoothed)
    SmoothedValue<float, ValueSmoothingTypes::Linear> smoothedVol2;     // osc 2 gain (smoothed)
    SmoothedValue<float, ValueSmoothingTypes::Linear> smoothedPw;       // pulse width (smoothed)


    /// NOISE ---------------------------------------------------
    Noise noise;
    float gain3 = 0.5f;                                                 // noise gain (current)
    std::atomic <float >* vol3;                                         // noise gain (raw)
    SmoothedValue<float, ValueSmoothingTypes::Linear> smoothedVol3;     // noise gain (smoothed)

    // ENVELOPE -------------------------------------------------
    ADSR env;                                                           // amplitude envelope
    ADSR::Parameters envParams;                                         // amplitude envelope paramters
    std::atomic <float>* attack;                                        // attack time (s)
    std::atomic <float>* decay;                                         // decay time (s)
    std::atomic <float>* sustain;                                       // sustain level
    std::atomic <float>* release;                                       // release time (s)
    bool playing = false;                                               // is note playing
    bool ending = false;                                                // is note ending




};