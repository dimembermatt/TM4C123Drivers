/**
 * File name: TriangleProfile.c
 * Devices: LM4F120; TM4C123
 * Description: Triangle wave with 6-bit resolution.
 * Authors: Matthew Yu.
 * Last Modified: 03/10/21
 */

/** Device specific imports. */
#include "TriangleProfile.h"


/**
 * Triangle Profile for audio generation. 64 element resolution with 64
 * element domain.
 */
const uint8_t TriangleProfile[64] = {
	32,34,36,38,40,42,44,48,50,52,54,56,58,60,62,63,
	62,60,58,56,54,52,50,48,46,44,42,40,38,36,34,32,
	30,28,26,24,22,20,18,16,14,12,10,8,6,4,2,0,
	2,4,6,8,10,12,14,16,18,20,22,24,26,28,30,32
};
