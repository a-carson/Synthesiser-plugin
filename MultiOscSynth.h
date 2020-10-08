#pragma once

#ifndef MultiOscSynth_h
#define MultiOscSynth_h
#include "Osc.h"

/*
A class for creating mutiple oscillators of different types
*/
class MultiOscSynth
{
public:
    /*
    Constructor
    @param count (number of oscillators)
    */
    MultiOscSynth(int count)
    {
        numOscs = count;
        for (int i = 0; i < numOscs; i++)
        {
            sawOscs.add(new SawtoothOsc);
            sineOscs.add(new SineOsc);
            squareOscs.add(new SquareOsc);
            triOscs.add(new TriOsc);
        }
    
    }

    /*
    Returns the output from a specific oscillator of a specific type
    @param osc
    @param oscType
    */
    float process(int osc, int oscType)
    { 
        for (int i = 0; i < numOscs; i++)
        {
            if (i == osc)
            {
                switch (oscType)
                {
                    case 0:
                        return sawOscs[i]->process() * 0.5f;
                    case 1:
                        return sineOscs[i]->process();
                    case 2:
                        return squareOscs[i]->process() * 0.5f;
                    case 3:
                        return triOscs[i]->process() * 0.5f;
                }
            }
        }

    }

    // ----------------------------------------------------------------------------
    /**
    Sets the sample rates for all oscillators
    @param sampleRate
    */
    void setSampleRate(float sampleRate)
    {

        for (int i = 0; i < numOscs; i++)
        {
            sawOscs[i]->setSampleRate(sampleRate);
            sineOscs[i]->setSampleRate(sampleRate);
            squareOscs[i]->setSampleRate(sampleRate);
            triOscs[i]->setSampleRate(sampleRate);
        }

    }

    //---------------------------------------------------------------------------
    /*
    Sets the frequency of a specific oscillator
    @param osc
    @param freq
    */
    void setOscFrequency(int osc, int oscType, float freq)
    {
        for (int i = 0; i < numOscs; i++)
        {
            if (i == osc)
            {
                if (oscType == 0)
                    sawOscs[i]->setFrequency(freq);

                if (oscType == 1)
                    sineOscs[i]->setFrequency(freq);

                if (oscType == 2)
                    squareOscs[i]->setFrequency(freq);

                if (oscType == 3)
                    triOscs[i]->setFrequency(freq);
            }
        }
    }

    void setPulseWidth(int osc, float pw)
    {
        for (int i = 0; i < numOscs; i++)
        {
            if (i == osc)
            {
                squareOscs[i]->setPulseWidth(pw);
            }
        }
    }

private:
    float sampleRate;
    float freq;
    int numOscs;
    OwnedArray<SawtoothOsc> sawOscs;
    OwnedArray<SineOsc> sineOscs;
    OwnedArray<SquareOsc> squareOscs;
    OwnedArray<TriOsc> triOscs;
};

#endif /*MultiOscSynth_h*/