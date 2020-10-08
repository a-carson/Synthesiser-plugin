#pragma once

#ifndef Phaser_h
#define Phaser_h
#include "Osc.h"
#include "Lfo.h"

class Phaser
{
public:
	Phaser(int stages)
	{
		numStages = stages;

		for (int i = 0; i < numStages; i++)
		{
			allPassFilters.add(new IIRFilter);
		}

		lfo.setDepth(1.0f);
	}

	float process(float input)
	{
		float lfoVal = 500 * lfo.process();
		float out = input;

		for (int i = 0; i < numStages; i++)
		{
			allPassFilters[i]->setCoefficients(IIRCoefficients::makeAllPass(sr, lfoVal));
			out += allPassFilters[i]->processSingleSampleRaw(out);
		}

		return out;
	}
	void setSampleRate(float sampleRate)
	{
		sr = sampleRate;
		lfo.setSampleRate(sampleRate);
	}

	void setLfoFreq(float freq)
	{
		lfo.setFrequency(freq);
	}

	void setDepth(float depth)
	{
		lfo.setDepth(depth);
	}
	

private:
	int numStages;
	OwnedArray<IIRFilter> allPassFilters;
	float sr;
	Lfo lfo;
};

#endif /*Phaser_h*/