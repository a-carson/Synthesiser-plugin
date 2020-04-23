#pragma once

#ifndef MultiOscSynth_h
#define MultiOscSynth_h
#include "Oscillator.h"

class MultiOscSynth
{
public:
    MultiOscSynth(int count)
    {
        numOscs = count;
        for (int i = 0; i < numOscs; i++)
        {
            sawOscs.add(new Phasor);
            sineOscs.add(new SineOsc);
            squareOscs.add(new SquareOsc);
            triOscs.add(new TriOsc);
        }
    
    }


    float process(int osc, int oscType)
    { 
        for (int i = 0; i < numOscs; i++)
        {
            if (i == osc)
            {
                if (oscType == 0)
                    return sawOscs[i]->process() * 0.5;
                if (oscType == 1)
                    return sineOscs[i]->process();
                if (oscType == 2)
                    return squareOscs[i]->process() * 0.5;
                if (oscType == 3)
                    return triOscs[i]->process();
            }
        }

    }


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

    void setOscFrequency(int osc, float freq)
    {
        for (int i = 0; i < numOscs; i++)
        {
            if (i == osc)
            {
                sawOscs[i]->setFrequency(freq);
                sineOscs[i]->setFrequency(freq);
                squareOscs[i]->setFrequency(freq);
                triOscs[i]->setFrequency(freq);
            }
        }
    }



private:
    float sampleRate;
    float freq;
    int numOscs;
    OwnedArray<Phasor> sawOscs;
    OwnedArray<SineOsc> sineOscs;
    OwnedArray<SquareOsc> squareOscs;
    OwnedArray<TriOsc> triOscs;
};

#endif /*TwoOscSynth_h*/