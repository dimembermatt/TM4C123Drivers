/**
 * Sound.h
 * Devices: LM4F120; TM4C123
 * Description: Low level drivers to produce sound waves from a DAC.
 * Authors: Matthew Yu.
 * Last Modified: 03/10/21
 **/
#pragma once

/** General imports. */
#include <stdint.h>
#include <stdio.h>

/** Device specific imports. */
#include "DAC.h"
#include "DACSPI.h"
#include <TM4C123Drivers/lib/Timers/Timers.h>


#define MAX_SOUND_ELEM 16


/** 
 * Sound configuration specifying the SSI or DAC that needs to be initialized.
 */
struct SoundConfig {
	/** Enumerator defining how the output format of the sound should be. */
	enum PlayerSource { R_DAC, SPI_DAC } source;
	
	union {
	    /** GPIO pin configuration. See DAC.h for more details. */
		DACConfig_t pins;

		/** SSI configuration. See SSI.h for more details. */
		SSIConfig_t ssi;
	} config;
};

void setCycle(void);

/**
 * initializeSoundPlayer sets up the timer and configures the output.
 * @param soundConfig The output format of the audio. Requires calling 
 * EnableInterrupts() after initialization.
 */
void initializeSoundPlayer(struct SoundConfig soundConfig);

/**
 * playSound plays a tone at a specified frequency and envelope for the provided
 * pins.
 * @param id        Identifier of the sound being played. Up to 5 unique
 *                  identifiers can be played. Useful for playing multiple
 *                  sounds at once. -1 is an invalid ID.
 * @param freq      Frequency of the sound being played. Up to 12kHz.
 * @param waveform  Reference to a 16 entry waveform where each entry is a value
 *                  from 0 to 4096. Sound envelope.
 */
void playSound(int8_t id, uint32_t freq, uint16_t* waveform);

/**
 * stopSound stops a tone from playing with a given id.
 * If the ID doesn't exist, do nothing.
 * @param id        Identifier of the sound being played.
 */
void stopSound(int8_t id);
