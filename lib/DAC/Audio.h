/**
 * Audio.h
 * Devices: LM4F120; TM4C123
 * Description: Low level drivers to produce audio from a DAC.
 * Authors: Matthew Yu.
 * Last Modified: 03/04/21
 **/
#pragma once

/** General imports. */
#include <stdint.h>
#include <stdio.h>

/** Device specific imports. */
#include "DAC.h"
#include <TM4C123Drivers/lib/Timers/Timers.h>


static uint8_t SineWave[64] = {
    32,35,38,41,44,47,49,52,54,56,58,59,
    61,62,62,63,63,63,62,62,61,59,58,56,
    54,52,49,47,44,41,38,35,32,29,26,23,
    20,17,15,12,10,8,6,5,3,2,2,1,1,1,2,
    2,3,5,6,8,10,12,15,17,20,23,26,29
};

static uint8_t SawtoothWave[64] = {
    0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,
    17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,
    33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,
    49,50,51,52,53,54,55,56,57,58,59,60,61,62,63
};

static uint8_t TriangleWave[64] = {
    32,34,36,38,40,42,44,48,50,52,54,56,58,60,62,63,
    62,60,58,56,54,52,50,48,46,44,42,40,38,36,34,32,
    30,28,26,24,22,20,18,16,14,12,10,8,6,4,2,0,
    2,4,6,8,10,12,14,16,18,20,22,24,26,28,30,32
};

static uint8_t Test[64] = {
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1,
};

/**
 * playSound plays a tone at a specified frequency and envelope for the provided
 * pins. Utilizes timer 2 for interrupts. Requires calling EnableInterrupts()
 * after initialization.
 * @param id        Identifier of the sound being played. Up to 5 unique
 *                  identifiers can be played. Useful for playing multiple
 *                  sounds at once.
 * @param freq      Frequency of the sound being played. Up to 2kHz.
 * @param waveform  Reference to a 64 entry waveform where each entry is a value
 *                  from 0 to 63. Sound envelope.
 * @param pins      The list of pins to configure the DAC with.
 */
void playSound(int8_t id, uint32_t freq, uint8_t* waveform, DACConfig pins);

/**
 * stopSound stops a tone from playing with a given id.
 * If the ID doesn't exist, do nothing.
 * @param id        Identifier of the sound being played.
 */
void stopSound(int8_t id);
