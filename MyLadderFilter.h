#ifndef LadderFilter_h
#define LadderFilter_h

#include <JuceHeader.h>
#include "Lfo.h"

/*Class which contains 4 instances of the JUCE ladder filter*/
class LadderFilter
{
public:
	LadderFilter()
	{
		for (int i = 0; i < 4; i++)
		{
			filters.add(new dsp::LadderFilter<float>);
		}
	}

	void process(int filterType, dsp::ProcessContextReplacing <float> context)
	{
		return filters[filterType]->process(context);
	}

	// Sets the process spec for all the filters and sets their modes
	void prepare(dsp::ProcessSpec spec)
	{
		for (int i = 0; i < 4; i++)
		{
			filters[i]->prepare(spec);

		}

		filters[0]->setMode(dsp::LadderFilter<float>::Mode::LPF12);
		filters[1]->setMode(dsp::LadderFilter<float>::Mode::LPF24);
		filters[2]->setMode(dsp::LadderFilter<float>::Mode::HPF12);
		filters[3]->setMode(dsp::LadderFilter<float>::Mode::HPF24);
	}

	// Sets the cut-off frequency from a normalised value
	void setNormalisedCutOff(float normalisedCutOffFreq)
	{
		float cutOff = pow(2, jmap(normalisedCutOffFreq, 4.32f, 14.32f));

		for (int i = 0; i < 4; i++)
		{
			if (cutOff > 0)
				filters[i]->setCutoffFrequencyHz(cutOff);
		}
	}

	// Sets the cut-off frequency in Hz
	void setCutOffFrequencyInHz(float frequency)
	{
		for (int i = 0; i < 4; i++)
		{
			filters[i]->setCutoffFrequencyHz(frequency);
		}
	}

	// Sets the filter resonance
	void setResonance(float q)
	{
		for (int i = 0; i < 4; i++)
		{
			filters[i]->setResonance(q);
		}
	}


private:
	OwnedArray < dsp::LadderFilter<float> > filters;		// array of filters
};
#endif /* LadderFilter_h */
