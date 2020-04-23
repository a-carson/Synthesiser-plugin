#ifndef LadderFilterWithLfo_h
#define LadderFilterWithLfo_h

#include <JuceHeader.h>
#include "Oscillator.h"

class LadderFilterWithLfo
{
public:
	void prepare(float sampleRate, dsp::ProcessSpec spec)
	{
		lowPassLadder.prepare(spec);
		highPassLadder.prepare(spec);
		lowPassLadder.setMode(dsp::LadderFilter<float>::Mode::LPF24);
		highPassLadder.setMode(dsp::LadderFilter<float>::Mode::HPF24);
		lfo.setSampleRate(sampleRate);
		lfo.setFrequency(*filterLfoFreqParam);
	}
private:

	dsp::LadderFilter<float > lowPassLadder;
	dsp::LadderFilter<float > highPassLadder;
	SineOsc lfo;
	float lfoFreq;
	float cutOff;
	float q;

};
#endif /* LadderFilterWithLfo_h */
