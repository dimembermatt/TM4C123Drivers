/**
 * Audio.c
 * Devices: LM4F120; TM4C123
 * Description: Low level drivers to produce audio from a DAC.
 * Authors: Matthew Yu.
 * Last Modified: 03/04/21
 **/

/** Device specific imports. */
#include "Audio.h"


struct SoundConfig {
    int8_t id;
    uint32_t freq;
    uint8_t* waveform;
    DACConfig pins;
    uint8_t position;
};

static struct SoundConfig sounds[] = {
    -1, 0, NULL, { PIN_COUNT }, 0,
    -1, 0, NULL, { PIN_COUNT }, 0,
    -1, 0, NULL, { PIN_COUNT }, 0,
    -1, 0, NULL, { PIN_COUNT }, 0,
    -1, 0, NULL, { PIN_COUNT }, 0,
};

static uint8_t soundsConfigured = 0;
static const uint8_t MAX_SOUNDS = 5;

/** Helper function called when we want to play one iteration of the sound. */
void iterateCycle(void) {
    static uint32_t cycle = 0;

    for (uint8_t i = 0; i < MAX_SOUNDS; ++i) {
        if (sounds[i].id != -1) {
            // Iteration frequency is 64 times of the tone frequency.
            // If the cycle is evenly divisible by the iteration frequency
            // period, then we output to DAC and execute the next interation.
            DACOut(sounds[i].waveform[sounds[i].position], sounds[i].pins);
            sounds[i].position = (sounds[i].position) % 64;
        }
    }

    cycle = (++cycle) % MAX_FREQ;
}

/**
 * playSound plays a tone at a specified frequency and envelope for the provided
 * pins. Utilizes timer 2a for interrupts. Requires calling EnableInterrupts()
 * after initialization.
 * @param id        Identifier of the sound being played. Up to 5 unique
 *                  identifiers can be played. Useful for playing multiple
 *                  sounds at once.
 * @param freq      Frequency of the sound being played. Up to 2kHz.
 * @param waveform  Reference to a 64 entry waveform where each entry is a value
 *                  from 0 to 63. Sound envelope.
 * @param pins      The list of pins to configure the DAC with.
 */
void playSound(int8_t id, uint32_t freq, uint8_t* waveform, DACConfig pins) {
    if (soundsConfigured == MAX_SOUNDS) return;    
    DACInit(pins);

    int8_t foundIdx = -1;
    int8_t newIdx = -1;
    uint32_t maxFreq = 0;
    // Find the max frequency, and where to put the sound in sounds.
    for (uint8_t i = 0; i < MAX_SOUNDS; ++i) {
        if (sounds[i].freq > maxFreq) {
            maxFreq = sounds[i].freq;
        }
        if (sounds[i].id == id) { 
            foundIdx = i;
        }
        if (sounds[i].id == -1 && newIdx == -1) { 
            newIdx = i;
        }
    }

    // We found an existing tone and are replacing it.
    if (foundIdx != -1) { 
        sounds[foundIdx].freq = freq;
        sounds[foundIdx].waveform = waveform;
        sounds[foundIdx].pins = pins;
        sounds[foundIdx].position = 0;
    } 
    // We did not find an existing tone, but we found an open slot to insert the
    // sound.
    else if (newIdx != -1) {
        sounds[newIdx].id = id;
        sounds[newIdx].freq = freq;
        sounds[newIdx].waveform = waveform;
        sounds[newIdx].pins = pins;
        sounds[newIdx].position = 0;   
    }
    // We didn't find an existing sound matching the ID nor an open spot. Do
    // nothing.
    else { return; }

    maxFreq = (freq > maxFreq) ? freq : maxFreq;

    // Initialize timer 2a.
    TimerInit(TIMER_2A, freqToPeriod(maxFreq*64, MAX_FREQ), iterateCycle);
}

/**
 * stopSound stops a tone from playing with a given id.
 * If the ID doesn't exist, do nothing.
 * @param id        Identifier of the sound being played.
 */
void stopSound(int8_t id) {
    for (uint8_t i = 0; i < MAX_SOUNDS; ++i) {
        if (sounds[i].id == id) {
            sounds[i].id = -1;
            sounds[i].freq = 0;
            sounds[i].waveform = NULL;
            for (uint8_t j = 0; j < 6; ++j) {
                sounds[i].pins.pinList[j] = PIN_COUNT;
            }
            sounds[i].position = 0;
            --soundsConfigured;
            return;
        }
    }
}
