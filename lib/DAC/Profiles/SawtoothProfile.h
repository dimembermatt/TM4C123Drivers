/**
 * File name: SawtoothProfile.h
 * Devices: LM4F120; TM4C123
 * Description: Sawtooth wave with 6-bit resolution.
 * Authors: Matthew Yu.
 * Last Modified: 03/10/21
 */
#pragma once

/** General imports. */
#include <stdint.h>


/**
 * Sawtooth Profile for audio generation. 64 element resolution with 64
 * element domain.
 */
const uint8_t SawtoothProfile[64];
