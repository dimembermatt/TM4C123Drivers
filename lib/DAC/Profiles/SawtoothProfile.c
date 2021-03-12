/**
 * File name: SawtoothProfile.c
 * Devices: LM4F120; TM4C123
 * Description: Sawtooth wave with 6-bit resolution.
 * Authors: Matthew Yu.
 * Last Modified: 03/10/21
 */

/** Device specific imports. */
#include "SawtoothProfile.h"


/**
 * Sawtooth Profile for audio generation. 64 element resolution with 64
 * element domain.
 */
const uint8_t SawtoothProfile[64] = {
    0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,
    17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,
    33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,
    49,50,51,52,53,54,55,56,57,58,59,60,61,62,63
};
