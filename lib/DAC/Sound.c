/**
 * Sound.c
 * Devices: LM4F120; TM4C123
 * Description: Low level drivers to produce sound waves from a DAC.
 * Authors: Matthew Yu.
 * Last Modified: 03/10/21
 **/

/** General imports. */
#include <stdbool.h>

/** Device specific imports. */
#include "Sound.h"


struct AudioConfiguration {
    int8_t id;
    uint32_t periodSteps;
    uint32_t currentPeriodSteps;
    uint16_t* waveform;
    union SoundConfig config;
    uint8_t position;
};

static struct AudioConfiguration sounds[] = {
    -1, 0, 0, NULL, { PIN_COUNT }, 0,
    -1, 0, 0, NULL, { PIN_COUNT }, 0,
    -1, 0, 0, NULL, { PIN_COUNT }, 0,
    -1, 0, 0, NULL, { PIN_COUNT }, 0,
    -1, 0, 0, NULL, { PIN_COUNT }, 0,
};

static uint8_t soundsConfigured = 0;
static const uint8_t MAX_SOUNDS = 5;
static enum PlayerSource source = R_DAC;
	
/** Helper function called when we want to play one iteration of the sound. */
void iterateCycle(void) {
    for (uint8_t i = 0; i < MAX_SOUNDS; ++i) {
        if (sounds[i].id != -1) {
            /* Increment the currentPeriodSteps. If it matches periodSteps, roll
             * over, play current position, and move forward one position. */
            sounds[i].currentPeriodSteps++;
            if (sounds[i].currentPeriodSteps >= sounds[i].periodSteps) {
                sounds[i].currentPeriodSteps = 0;
                /* TODO: this may not perform superposition of waves properly. */
                if (source == R_DAC)
                    DACOut(sounds[i].config.pins, sounds[i].waveform[sounds[i].position]);
                else {
                    //DACSPIOut(sounds[i].config.ssi.SSI, 65535);
                    DACSPIOut(sounds[i].config.ssi.SSI, sounds[i].waveform[sounds[i].position]);
                }
                sounds[i].position = (sounds[i].position + 1) % MAX_SOUND_ELEM;
            }
        }
    }
}

void playCurrentCycle(void);

/**
 * initializeSoundPlayer sets up the timer and configures the output.
 * @param playerSource The output format of the audio.
 */
void initializeSoundPlayer(enum PlayerSource playerSource) {
    /* Player is initialized to 400 kHz (2500 ns). */
    TimerInit(TIMER_1A, freqToPeriod(400000, MAX_FREQ), iterateCycle);
    source = playerSource;
}

/**
 * playSound plays a tone at a specified frequency and envelope for the provided
 * pins. Utilizes timer 2 for interrupts. Requires calling EnableInterrupts()
 * after initialization.
 * @param id        Identifier of the sound being played. Up to 5 unique
 *                  identifiers can be played. Useful for playing multiple
 *                  sounds at once.
 * @param freq      Frequency of the sound being played. Up to 2kHz.
 * @param waveform  Reference to a 32 entry waveform where each entry is a value
 *                  from 0 to 4096. Sound envelope.
 * @param config    Configuration. Either a DACConfig_t or SSIConfig_t.
 */
void playSound(uint8_t id, uint32_t freq, uint16_t* waveform, union SoundConfig config) {
    /* Exit early if the maximum number of sounds are already configured. */
    if (soundsConfigured == MAX_SOUNDS) return;    

    /**    
    Calcs:
    446 Hz event (A4) * 32 subevents = 14_272 Hz for each sound subevent
    Equivalent to 5605 12.5 ns (80 MHz) iterations
    Timer clock is 2500 ns (400 kHz); 5605 / 200 -> 28 2500 ns iterations.
    Max frequency is:z

    446 Hz         12.5 kHz 
    ------     =>   ------
    28 cycles       1 cycle

    Actual playable frequency will probably be closer to 10 kHz.
    */

    /* Convert freq into period for 1/32 notes. */
    uint32_t period = freqToPeriod(freq*MAX_SOUND_ELEM, MAX_FREQ);

    /* Convert into period steps; i.e. how many 2500 ns events. */
    period /= 200;

    int8_t foundIdx = -1;
    int8_t newIdx = -1;
    for (uint8_t i = 0; i < MAX_SOUNDS; ++i) {
        /* Find the existing sound, if any. */
        if (sounds[i].id == id) { 
            foundIdx = i;
        }

        /* Find the first empty slot in sounds, if any. */
        if (sounds[i].id == -1 && newIdx == -1) { 
            newIdx = i;
        }
    }

    /* We found an existing tone and are replacing it. */
    if (foundIdx != -1) { 
        sounds[foundIdx].periodSteps = period;
        sounds[foundIdx].currentPeriodSteps = 0;
        sounds[foundIdx].waveform = waveform;
        sounds[foundIdx].config = config;
        sounds[foundIdx].position = 0;
    } 
    /* We did not find an existing tone, but we found an open slot to insert the
     * sound. */
    else if (newIdx != -1) {
        sounds[newIdx].id = id;
        sounds[newIdx].periodSteps = period;
        sounds[newIdx].currentPeriodSteps = 0;
        sounds[newIdx].waveform = waveform;
        sounds[newIdx].config = config;
        sounds[newIdx].position = 0;   
    }
    /* We didn't find an existing sound matching the ID nor an open spot. Do
     * nothing. */
    else { return; }

    /* Configure the DAC. */
    if (source == R_DAC) DACInit(config.pins);
    else DACSPIInit(config.ssi);
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
            sounds[i].periodSteps = 0;
			sounds[i].currentPeriodSteps = 0;
            sounds[i].waveform = NULL;
            /* TODO: for now don't reset list. */
            // for (uint8_t j = 0; j < 6; ++j) {
            //     sounds[i].pins.pinList[j] = PIN_COUNT;
            // }
            sounds[i].position = 0;
            --soundsConfigured;
            return;
        }
    }
}
