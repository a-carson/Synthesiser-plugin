#pragma once

#ifndef Lfo_h
#define Lfo_h

#include "Osc.h"

// Straight forward sine oscillator which can be used as an LFO
class Lfo
{
public:

	// Returns the current value of the LFO
	float process()
	{
		return jmap(sineOsc.process(), -0.5f, 0.5f, min, max);
	}

	// Sets the sample rate
	void setSampleRate(float sampleRate)
	{
		sineOsc.setSampleRate(sampleRate);
	}

	// Sets the LFO frequency
	void setFrequency(float frequency)
	{
		freq = frequency;
		sineOsc.setFrequency(frequency);
	}

	// Sets the depth of the LFO
	void setDepth(float normalisedDepth)
	{
		min = 1 / (normalisedDepth + 1.0f);
		max = 1.0f + normalisedDepth;
	}

	// Sets the minimum and maximum of the LFO directly
	void setMinMax(float minimum, float maximum)
	{
		min = minimum;
		max = maximum;
	}

private:
	SineOsc sineOsc;
	float freq;
	float min;
	float max;
};


// Generates a semi-random oscillator by the summation of fourier components
class RandomLfo
{
public: 

	/*Constructor
	@param fourierComponents - number of sine oscs summed together to give "random" signal. Default = 10*/
	RandomLfo(int fourierComponents = 10)
	{
		numFourierComponents = fourierComponents;
		freqs = new float[numFourierComponents];

		for (int i = 0; i < numFourierComponents; i++)
		{
			lfos.add(new SineOsc);
		}

	}

	// Returns the current value of the LFO
	float process()
	{
		float out = 0;

		for (int i = 0; i < numFourierComponents; i++)
		{
			out += (1 / (numFourierComponents * 2.0f)) * lfos[i]->process();
		}

		return jmap(out, min, max);
	}

	// Sets the sample rate
	void setSampleRate(float sampleRate)
	{
		for (int i = 0; i < numFourierComponents; i++)
		{
			lfos[i]->setSampleRate(sampleRate);
		}
	}

	// Sets speed and depth simultaneously
	void setParameters(float speed, float depth)
	{
		setSpeed(speed);
		setDepth(depth);
	}

	// Sets the approximate speed of the LFO (this it not the exact frequency)
	void setSpeed(float speed)
	{
		for (int i = 0; i < numFourierComponents; i++)
		{
			freqs[i] = randomNumber.nextFloat() * speed;
			lfos[i]->setFrequency(freqs[i]);
		}
	}

	// Sets the maximum depth of the LFO
	void setDepth(float normalisedDepth)
	{
		min = 1 / (normalisedDepth + 1.0f);
		max = 1.0f + normalisedDepth;
	}

	// Sets the minimum and maximum of the LFO directly
	void setMinMax(float minimum, float maximum)
	{
		min = minimum;
		max = maximum;
	}

private:
	OwnedArray<SineOsc> lfos;			// array of sine oscs
	float* freqs;						// array of semi-random frequencies
	Random randomNumber;				// random number generator for sine wave frequencies
	int numFourierComponents;			// number of sine waves
	float min;					
	float max;
	
};
#endif /* Lfo_h */