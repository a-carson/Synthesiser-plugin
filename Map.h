#pragma once

#ifndef Map_h
#define Map_h

// MAPPING FUNCTION /////////////////////////////////////////////////////
/** Maps a value to a new range
@param inputValue		the value to map to a new range
@param oldMinimum		minimum value of current range
@param oldMaximum		maximum value of current range
@param newMinimum		minimum value of desired range
@param newMaximum		maximum value of desired range

@returns a float within the new range
*/
float map(float inputValue, float oldMinimum, float oldMaximum, float newMinimum, float newMaximum);

#endif /*Map_h*/