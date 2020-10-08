#pragma once

#ifndef DelayLine_h
#define DelayLine_h
#include "Osc.h"
#include "Lfo.h"

/** Stores a sample in a buffer and reads it after a specified time. 
Uses Lagrange interpolation to account for fractional delays.*/
class DelayLine
{
public:
	/*DelayLine constructor*/
	DelayLine()
	{
		// Initialise arrays
		for (int i = 0; i < numNearestNeighbours; i++)
		{
			readHeads[i] = 0;
			basisFunctionValues[i] = 0;
			readValues[i] = 0.0f;
		}
	}

	/*Delays the input sample by the specified delay time and returns it*/
	float process(float currentSampleValue)
	{
		storeValue(currentSampleValue);
		return readValue();
	}

	/*Stores the current sample value in the buffer*/
	void storeValue(float sample)
	{
		// Writes current sample in buffer + the feedback component
		buffer[writeHead] = sample + feedback * readValue();

		// Calculate exact delay time in samples
		float fractionalDelay = writeHead * 1.0f - (0.001f * delayInMiliSeconds * sampleRate);

		// 4 nearest read heads for lagrange interpolation
		readHeads[0] = (int) (floor(fractionalDelay) - 1.0f);
		readHeads[1] = (int) floor(fractionalDelay);
		readHeads[2] = (int) ceil(fractionalDelay);
		readHeads[3] = (int) (ceil(fractionalDelay) + 1.0f);

		// alpha value for lagrage interpolation - a measure of the proximity to the neighbouring samples
		alpha = fractionalDelay - 1.0f*readHeads[1] - 0.5f;

		// Move write head along buffer and wrap if necessary
		writeHead++;
		writeHead %= bufferSize;
	}

	/*Reads the delayed sample value using Lagrange interpolation*/
	float readValue()
	{
		// Loop through read heads
		for (int i = 0; i < numNearestNeighbours; i++)
		{
			// Prevent negative read head values
			while (readHeads[i] < 0)
				readHeads[i] += bufferSize;

			// Read buffer at read heads
			readValues[i] = buffer[readHeads[i]];

			// Move read heads buffer and wrap
			readHeads[i]++;
			readHeads[i] %= bufferSize;
		}

		// Basis functions for lagrange interpolation
		basisFunctionValues[0] = (alpha + 0.5f) * (alpha - 0.5f) * (alpha - 1.5f) / -6.0f;
		basisFunctionValues[1] = (alpha + 1.5f) * (alpha - 0.5f) * (alpha - 1.5f) / 2.0f;
		basisFunctionValues[2] = (alpha + 0.5f) * (alpha - 1.5f) * (alpha + 1.5f) / -2.0f;
		basisFunctionValues[3] = (alpha + 0.5f) * (alpha - 0.5f) * (alpha + 1.5f) / 6.0f;

		float output = 0;

		// Multiply each read value by corresponding basis function and sum
		for (int i = 0; i < numNearestNeighbours; i++)
		{
			output += readValues[i]*basisFunctionValues[i];
		}

		return output;

	}

	/*Sets the sample rate (Hz)*/
	void setSampleRate(float fs)
	{
		sampleRate = fs;
	}

	/*Sets the size of the buffer in miliseconds*/
	void setMaxDelayTime(float maxDelayTime)
	{
		maxDelayTimeInMiliSeconds = maxDelayTime;
		bufferSize = (int) ceil(0.001f * maxDelayTimeInMiliSeconds * sampleRate);
		buffer = new float[bufferSize];

		// Initialise buffer with zeros
		for (int i = 0; i < bufferSize; i++)
		{
			buffer[i] = 0.0f;
		}
	}

	/*Delay time in miliseconds - must not exceed the maximum delay time*/
	void setDelayTime(float delayTime)
	{
		delayInMiliSeconds = delayTime;

		// Ensure we don't read outside the buffer
		if (delayInMiliSeconds >= maxDelayTimeInMiliSeconds)
			delayInMiliSeconds = maxDelayTimeInMiliSeconds;
	}

	/*Sets feedback amount - between 0 and 1*/
	void setFeedbackAmount(float fb)
	{
		feedback = fb;

		if (feedback >= 1.0f)
			feedback = 1.0f;

		if (feedback <= 0.0f)
			feedback = 0.0f;
	}

/*Destructor*/
	~DelayLine()
	{
		delete buffer;
		delete readHeads;
		delete readValues;
		delete basisFunctionValues;
	}

private:
	float sampleRate;												// sample rate in Hz
	float delayInMiliSeconds;										// delay time in miliseconds
	float maxDelayTimeInMiliSeconds;								// buffer size in miliseconds
	int bufferSize = 1000;											// buffer size
	float* buffer;													// pointer to buffer
	int numNearestNeighbours = 4;									// for lagrange interpolation
	float alpha = 0.0f;												// proximity of fractional delay to neighbouring samples
	int writeHead = 0;												// index in which to store current sample
	int* readHeads = new int[numNearestNeighbours];					// indices to read from
	float* readValues = new float[numNearestNeighbours];			// values at above indices, weighted according to basis function values (below)
	float* basisFunctionValues = new float[numNearestNeighbours];	// basis function values. Depends on alpha
	float feedback = 0.5f;											// feedback amount (beetween 0 and 1)
};


/*Flanger effect - specify TRUE in the constructor for random LFO. Default is FALSE.*/
class Flanger
{

public: 
	/* 
	Constructor
	@param random (set true for random lfo)
	*/
	Flanger(bool random = NULL)
	{
		isRandom = random;
	}

	void initialise(float sampleRate, float maxDelayTimeInMiliSeconds)
	{
		delay.setSampleRate(sampleRate);
		delay.setMaxDelayTime(1.10f * maxDelayTimeInMiliSeconds);
		lfo.setSampleRate(sampleRate);
		randLfo.setSampleRate(sampleRate);
	}
	/*Sets all required parameters for the flanger.
	
	@param sampleRate	in Hz
	@param lfoFrequency	in Hz
	@param delayTime	average delay time in miliseconds. LFO oscillates between 0 and 2 * delayTime.
	@param strength		mix of wet effect (between 0 and 1)
	@param feedback		feedback amount (between 0 and 1)
	*/
	void setParameters(float delayTime,float lfoFrequency, float depth, float feedback = 0.0f)
	{
		delay.setFeedbackAmount(feedback);

		if (isRandom == false)
		{
			lfo.setFrequency(lfoFrequency);
			lfo.setDepth(depth);
		}

		if (isRandom == true)
		{
			randLfo.setParameters(lfoFrequency, depth);
		}

		delayInMiliSeconds = delayTime;
	}


	/*Returns the input sample + delayed sample to give flanger effect*/
	float process(float sample)
	{
		if (isRandom == false)
			delay.setDelayTime(lfo.process() * delayInMiliSeconds);
		
		if (isRandom == true)
			delay.setDelayTime(randLfo.process() * delayInMiliSeconds);

		float output = delay.process(sample);
		return  output;
	}

private:
	bool isRandom;
	DelayLine delay;			// delay line
	Lfo lfo;					// lfo sine osc
	RandomLfo randLfo;			// random lfo
	float lfoFreq;				// LFO frequency in Hz
	float delayInMiliSeconds;	// average delay time in miliseconds. LFO oscillates between 0 and 2 * delayTime.
};

/*Multi-channel chorus class. Specify number of channels in the constructor.
  The delay times are controlled by the RandomLFO class.*/
class Chorus
{
public:
	/*Constructor
	@param numChannels*/ 
	Chorus(int numChannels)
	{
		channels = numChannels;

		for (int i = 0; i < channels; i++)
		{
			flangers.add(new Flanger(true));
		}
	}

	// Returns the current value of the delayed signal - can specify a particular channel is desired.
	float process(float sample, int channel = NULL)
	{
		float out = 0.0f;

		for (int i = 0; i < channels; i++)
		{
			if (i == channel)
				return flangers[channel]->process(sample);

			else
			{
				out += flangers[i]->process(sample);
			}

		}

		return out;
	}

	// Sets the sample rate and maximum buffer size
	void initialise(float sampleRate, float maxDelayTimeInMilliseconds)
	{
		for (int i = 0; i < channels; i++)
		{
			flangers[i]->initialise(sampleRate, maxDelayTimeInMilliseconds);
		}
	}

	// Sets all parameters at once
	void setParameters(float delayTime, float speed, float depth = 1.0f)
	{
		rate = speed;
		dept = depth;
		delay = delayTime;

		for (int i = 0; i < channels; i++)
		{
			flangers[i]->setParameters(delayTime, speed, depth);
		}
	}

	// Sets the speed of the chorus effect
	void setSpeed(float speed)
	{
		rate = speed;

		for (int i = 0; i < channels; i++)
		{
			flangers[i]->setParameters(delay, speed, dept);
		}
	}

	// Sets the average delay time of the chorus effect
	void setDelayTime(float delayTime)
	{
		delay = delayTime;

		for (int i = 0; i < channels; i++)
		{
			flangers[i]->setParameters(delay, rate, dept);
		}
	}

private:
	OwnedArray<Flanger> flangers;	// array of flangers, 1 per channel
	int channels;					// number of channels
	float delay;					// average delay time
	float rate;						// speed of LFO
	float dept;						// depth of LFO
};
#endif /* DelayLine_h */


