/**
 * File name: TriangleProfile.h
 * Devices: LM4F120; TM4C123
 * Description: Triangular wave with 6-bit resolution.
 * Authors: Matthew Yu.
 * Last Modified: 03/10/21
 */
#pragma once

/** General imports. */
#include <stdint.h>


/**
 * Triangle Profile for audio generation. 64 element resolution with 64
 * element domain.
 */
extern const uint8_t TriangleProfile[64];
