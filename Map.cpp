#include "Map.h"

float map(float inputValue, float oldMinimum, float oldMaximum, float newMinimum, float newMaximum)
{
	return ((inputValue - oldMinimum) / (oldMaximum - oldMinimum)) * (newMaximum - newMinimum) + newMinimum;
}

