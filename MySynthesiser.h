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

    void setParameterPointers(std::atomic <float>* param1, std::atomic <float>* param2, std::atomic <float>* param3, std::atomic <float>* param4, std::atomic <float>* param5)
    {
        detuneAmount = param1;
        attack = param2;
        decay = param3;
        sustain = param4;
        release = param5;
    }

    void initialise(float sampleRate)
    {
        // Oscillators
        triOsc.setSampleRate(sampleRate);
        detuneOsc.setSampleRate(sampleRate);

        // Envelope
        env.setSampleRate(sampleRate);
        envParams.attack = 0.1f;
        envParams.decay = 0.1f;
        envParams.sustain = 1.0f;
        envParams.release = 0.01f;
        env.setParameters(envParams);
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
        playing = true;
        freq = MidiMessage::getMidiNoteInHertz(midiNoteNumber);
        triOsc.setFrequency(freq);
        ending = false;
        env.reset();
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
            detuneOsc.setFrequency(freq * (1.0f - *detuneAmount));


            // iterate through the necessary number of samples (from startSample up to startSample + numSamples)
            for (int sampleIndex = startSample; sampleIndex < (startSample + numSamples); sampleIndex++)
            {
                /*envParams.attack = *attack;
                envParams.decay = *decay;
                envParams.sustain = *sustain;
                envParams.release = *release;
                env.setParameters(envParams);*/

                float envVal = env.getNextSample();
                float currentSample = (triOsc.process() + 0.5 * detuneOsc.process())* envVal;
                

                // for each channel, write the currentSample float to the output
                for (int chan = 0; chan < outputBuffer.getNumChannels(); chan++)
                {
                    // The output sample is scaled by 0.2 so that it is not too loud by default
                    outputBuffer.addSample(chan, sampleIndex, currentSample * 0.2);
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

    TriOsc triOsc;
    TriOsc detuneOsc;
    float freq;
    std::atomic <float>* detuneAmount;
    ADSR env;
    ADSR::Parameters envParams;
    std::atomic <float>* attack;
    std::atomic <float>* decay;
    std::atomic <float>* sustain;
    std::atomic <float>* release;

    

    /// a random object for use in our test noise function
    Random random;
};