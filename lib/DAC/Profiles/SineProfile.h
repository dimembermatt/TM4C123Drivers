/**
 * File name: SineProfile.h
 * Devices: LM4F120; TM4C123
 * Description: Sinusoidal wave with 6-bit resolution.
 * Authors: Matthew Yu.
 * Last Modified: 03/10/21
 */
#pragma once

/** General imports. */
#include <stdint.h>


/**
 * Sinusoidal Profile for audio generation. 64 element resolution with 64
 * element domain.
 */
extern uint8_t SineProfile[64];
