#pragma once

#ifndef Lfo_h
#define Lfo_h

#include "Oscillator.h"

class Lfo
{
public:
	float process()
	{
		return jmap(sineOsc.process(), -0.5f, 0.5f, min, max);
	}
	void setSampleRate(float sampleRate)
	{
		sr = sampleRate;
		sineOsc.setSampleRate(sr);
	}

	void setFrequency(float frequency)
	{
		freq = frequency;
		sineOsc.setFrequency(frequency);
	}

	void setDepth(float normalisedDepth)
	{
		min = 1 / (normalisedDepth + 1.0f);
		max = 1.0f + normalisedDepth;
	}

	void setMinMax(float minimum, float maximum)
	{
		min = minimum;
		max = maximum;
	}

private:
	SineOsc sineOsc;
	float sr;
	float freq;
	float min;
	float max;
	float depth;
	float baselineVal;
};
#endif /* Lfo_h */