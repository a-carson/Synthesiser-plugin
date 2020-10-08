#pragma once


#ifndef Osc_h
#define Osc_h
#include <cmath>

/*Generates a sawtooth wave.*/
class Phasor
{
public:

    /*Initialise values*/
    Phasor() : frequency(0.0f), 
               sampleRate(44100.0f), 
               phase(0.0f), 
               phaseDelta(0.0f), 
               pShift(0.0f)
    {}

    /*Returns the current value of the wave*/
    float process()
    {
        phase += phaseDelta;

        float shiftedPhase = phase + pShift;

        if (phase > 1.0f)
            phase -= 1.0f;

        if (shiftedPhase > 1.0f)
            shiftedPhase -= 1.0f;

        return output(shiftedPhase);
    }

    virtual float output(float p)
    {
        return p;
    }

    /*Sets the sample rate*/
    void setSampleRate(float sr)
    {
        sampleRate = sr;
    }

    /*Sets the frequency*/
    void setFrequency(float freq)
    {
        frequency = freq;
        phaseDelta = frequency / sampleRate;
    }

    /*Sets the offset of the phasor*/
    void setPhaseShift(float phaseShift)
    {
        pShift = phaseShift;
    }

float phaseDelta;

private:
    float frequency;
    float sampleRate;
    float phase;
    float pShift;
    float currentPhaseShift = 0.0f;
};

/*Generates a sine wave*/
class SineOsc : public Phasor
{
    float output(float p) override
    {
        return 0.5f * sin(2 * 3.1459f * p);
    }
};


/*Generates an anti-aliased sawtooth wave*/
class SawtoothOsc : public Phasor
{
    float output(float p) override
    {
        bphase = 2.0f * p - 1.0f;               // set phase to count from -1 to 1
        sq = pow(bphase, 2);                    // square
        deriv = (sq - sqPrev)/phaseDelta;       // differentiate
        sqPrev = sq;                            // update state variable
        return deriv * 0.125f;                  // scale output
    }

private:
    float bphase = 0.0f;                        // new phase expression
    float sq = 0.0f;                            // phase squared    
    float deriv = 0.0f;                         // derivative approximation
    float sqPrev = 0.0f;                        // previous phase squared value
};

/*Generates an anti-aliased square wave from two out-of-phase sawtooth waves*/
class SquareOsc
{
public:
    /*Constructor*/
    SquareOsc()
    {
        setPulseWidth(0.5f);
    }

    /*Returns the current value of the square wave*/
    float process()
    {
        return saw1.process() - saw2.process();
    }

    /*Sets the sample rate*/
    void setSampleRate(float sampleRate)
    {
        saw1.setSampleRate(sampleRate);
        saw2.setSampleRate(sampleRate);
    }

    /*
    Sets the pulse width
    @param pw - between 0 and 0.5
    */
    void setPulseWidth(float pw)
    {
        saw2.setPhaseShift(pw);
    }

    /*Sets the frequency*/
    void setFrequency(float freq)
    {
        saw1.setFrequency(freq);
        saw2.setFrequency(freq);
    }

private:
    SawtoothOsc saw1;       // sawtooth oscillator 1
    SawtoothOsc saw2;       // sawtooth oscillator 2
};


class TriOsc
{
public:

    float process()
    {
        phase += 2 * phaseDelta;

        if (phase > 1.0f)
        {
            phase -= 1.0f;
            squ *= -1.0f;
        }

        bphase = 2.0f * phase - 1.0f;
        parabolic = (1.0f - pow(bphase, 2)) * squ;
        deriv = (parabolic - parabolicPrev) / phaseDelta;
        parabolicPrev = parabolic;
        return deriv * 0.125f;
    }

    void setSampleRate(float sr)
    {
        sampleRate = sr;
    }

    void setFrequency(float freq)
    {
        frequency = freq;
        phaseDelta = frequency / sampleRate;
    }

private:
    float phase = 0.0f;
    float phaseDelta;
    float sampleRate;
    float frequency;
    float parabolicPrev = 0.0f;
    float parabolic;
    float bphase;
    float squ = -1.0;
    float deriv;
};

/*Simple white noise class*/
class Noise
{
public:
    /*Get next random sample - between -0.25 and 0.25*/
    float process()
    {
        return 0.5f * (random.nextFloat() - 0.5f);
    }

private:
    Random random;  // random number variable
};
#endif /* Osc_h */

