/**
 * Audio.c
 * Devices: LM4F120; TM4C123
 * Description: Low level drivers to produce audio from a DAC.
 * Authors: Matthew Yu.
 * Last Modified: 03/03/21
 **/

/** General imports. */
#include <stdint.h>

/** Device specific imports. */
#include "DAC.h"

void playSineWave(pins) {
    static uint8_t iterator = 0;
    
    DACOut(SineWave[++iterator % 64], pins);
}

typedef struct soundConfig {
    uint8_t id;
    uint32_t freq;
    uint8_t* waveform;
    DACConfig pins;
    uint8_t position;
} SoundConfig;

static SoundConfig sounds {
    -1, 0, NULL, { PIN_COUNT }, 0,
    -1, 0, NULL, { PIN_COUNT }, 0,
    -1, 0, NULL, { PIN_COUNT }, 0,
    -1, 0, NULL, { PIN_COUNT }, 0,
    -1, 0, NULL, { PIN_COUNT }, 0,
};

static uint8_t soundsConfigured = 0;
static const MAX_SOUNDS = 5;

// TODO: handler that processes through sounds.
void Timer2A_Handler(void) {

}

/**
 * playSound plays a tone at a specified frequency and envelope for the provided
 * pins. Utilizes timer 2 for interrupts.
 * @param id        Identifier of the sound being played. Up to 5 unique
 *                  identifiers can be played. Useful for playing multiple
 *                  sounds at once.
 * @param freq      Frequency of the sound being played. Up to 2kHz.
 * @param waveform  Reference to a 64 entry waveform where each entry is a value
 *                  from 0 to 63. Sound envelope.
 * @param pins      The list of pins to configure the DAC with.
 */
void playSound(uint8_t id, uint32_t freq, uint8_t* waveform, DACConfig pins) {
    if (soundsConfigured == MAX_SOUNDS) return;    
    DACInit(pins);
    // TODO: Copy parameters into sounds.
    // TODO: determine max frequency from sounds.
    // Initialize timer 2a.
    Timer2AInit(maxFreq);
}

/**
 * stopSound stops a tone from playing with a given id.
 * @param id        Identifier of the sound being played.
 */
void stopSound(uint8_t id) {
    // TODO: Clean out sounds of entry.

    --soundsConfigured;
}
