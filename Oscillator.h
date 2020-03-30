#pragma once


#ifndef Oscillator_h
#define Oscillator_h
#include <cmath>
#include "Map.h"

/*Generates a sawtooth wave.*/
class Phasor
{
public:

    /*Returns the current value of the wave*/
    float process()
    {
        phase += phaseDelta + phaseShift;

        if (phase > 1.0f)
            phase -= 1.0f;

        return output(phase);
    }

    virtual float output(float p)
    {
        return p;
    }

    /*Sets the sample rate*/
    void setSampleRate(float Fs)
    {
        sampleRate = Fs;
    }

    /*Sets the frequency*/
    void setFrequency(float freq)
    {
        frequency = freq;
        phaseDelta = frequency / sampleRate;
    }

    /*Sets the phase shift*/
    void setPhaseShift(float phShft)
    {
        phaseShift = phShft;
    }

    /*Sets the starting phase*/
    void setPhaseOffset(float startingPhase)
    {
        phase = startingPhase / 2.0f;
    }

    /*Sets the pulse width.
    Only relevant to SquareOsc and TriSquareOsc classes.*/
    void setPulseWidth(float pulseWidth)
    {
        pulseWidth *= 0.001f * phaseDelta*sampleRate;
        threshold = 1.0f - pulseWidth;

        if (threshold < 0)
            threshold = 0.0f;

        if (threshold > 1)
            threshold = 1.0f;


    }


    float threshold = 0.5f;

private:
    float frequency;
    float sampleRate;
    float phase = 0.0f;
    float phaseDelta;
    float phaseShift = 0.0f;
};

/*Generates a triangular wave*/
class TriOsc : public Phasor
{
    float output(float p) override
    {
        return fabsf(p - 0.5f) - 0.5f;
    }
};

/*Generates a sine wave*/
class SineOsc : public Phasor
{
    float output(float p) override
    {
        return 0.5f * sin(2 * 3.1459 * p);
    }
};

/*Generates a cosine wave*/
class CosOsc : public Phasor
{
    float output(float p) override
    {
        return 0.5f * cos(2 * 3.1459 * p);
    }
};

/*Generates a square wave*/
class SquareOsc : public Phasor
{
    float output(float p) override
    {
        if (p > threshold)
            p = -0.5f;

        else
            p = 0.5f;

        return p;
    }

};

/*Generates a triangular/square wave hybrid*/
class TriSquareOsc : public Phasor
{
    float output(float p) override
    {
        if (p > threshold)
            p = -0.5f;

        return p;
    }
};



#endif /* Oscillators_h */

