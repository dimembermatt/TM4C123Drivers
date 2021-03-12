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
uint8_t SineProfile[64] = {
    32,38,44,49,54,58,
    61,62,63,62,61,58,
    54,49,44,38,32,26,
    20,15,10,6,3,2,1,2,
    2,5,8,12,17,23,29
};
