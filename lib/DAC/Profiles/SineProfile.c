/**
 * File name: SineProfile.c
 * Devices: LM4F120; TM4C123
 * Description: Sinusoidal wave with 6-bit resolution.
 * Authors: Matthew Yu.
 * Last Modified: 03/10/21
 */

/** Device specific imports. */
#include "SineProfile.h"


/**
 * Sinusoidal Profile for audio generation. 64 element resolution with 64
 * element domain.
 */
uint8_t SineProfile[16] = {
	0, 1, 3, 6, 9, 12, 14, 15,
	15, 14, 12, 9, 6, 3, 1, 0,
};
