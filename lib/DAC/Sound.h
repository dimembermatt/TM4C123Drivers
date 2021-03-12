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


#define MAX_SOUND_ELEM 32

/** Enumerator defining how the output format of the sound should be. */
enum PlayerSource { R_DAC, SPI_DAC };

/** 
 * Sound configuration specifying the SSI or DAC that needs to be initialized.
 */
union SoundConfig {
    /** GPIO pin configuration. See DAC.h for more details. */
    DACConfig_t pins;

    /** SSI configuration. See SSI.h for more details. */
    SSIConfig_t ssi;
};

/**
 * initializeSoundPlayer sets up the timer and configures the output. 
 * Utilizes timer 2a for interrupts. Requires calling EnableInterrupts() 
 * after initialization. Maximum 12.5 kHz frequency.
 * @param playerSource The output format of the audio R_DAC (resistor based DAC)
 *                     or SPI_DAC (SPI based DAC).
 */
void initializeSoundPlayer(enum PlayerSource playerSource);

/**
 * playSound plays a tone at a specified frequency and envelope for the provided
 * pins. Utilizes Timer 2A for interrupts. Requires calling EnableInterrupts()
 * after initialization.
 * @param id        Identifier of the sound being played. Up to 5 unique
 *                  identifiers can be played. Useful for playing multiple
 *                  sounds at once.
 * @param freq      Frequency of the sound being played. Up to 12.5 kHz.
 * @param waveform  Reference to a 32 entry waveform where each entry is a value
 *                  from 0 to 4096. Should have the sound envelope pre-applied.
 * @param config    Configuration. Either a DACConfig_t or SSIConfig_t.
 */
void playSound(uint8_t id, uint32_t freq, uint16_t* waveform, union SoundConfig config);

/**
 * stopSound stops a tone from playing with a given id.
 * If the ID doesn't exist, do nothing.
 * @param id        Identifier of the sound being played.
 */
void stopSound(int8_t id);
