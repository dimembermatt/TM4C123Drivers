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


struct SoundStorage {
	int8_t id;
	uint32_t periodSteps;
	uint32_t currentPeriodSteps;
	uint8_t* waveform;
	uint8_t position;
};

static struct SoundStorage sounds[] = {
	{-1, 0, 0, NULL, 0},
	{-1, 0, 0, NULL, 0},
	{-1, 0, 0, NULL, 0},
	{-1, 0, 0, NULL, 0},
	{-1, 0, 0, NULL, 0},
};

static uint8_t soundsConfigured = 0;
static const uint8_t MAX_SOUNDS = 5;
static struct SoundConfig soundConfig;
	
/** Helper function called when we want to play one iteration of the sound. */
void iterateCycle(void) {
	for (uint8_t i = 0; i < MAX_SOUNDS; ++i) {
		if (sounds[i].id != -1) {
			/* Increment the currentPeriodSteps. If it matches periodSteps, roll
			 * over, play current position, and move forward one position. */
			sounds[i].currentPeriodSteps = (sounds[i].currentPeriodSteps + 1) % sounds[i].periodSteps;
			if (sounds[i].currentPeriodSteps == 0) { /* If current period step = 0 */
				if (soundConfig.source == R_DAC) {
					DACOut(soundConfig.config.pins, sounds[i].waveform[sounds[i].position]);
				} else {
					//DACSPIOut(soundConfig.config.ssi.SSI, 10);
					DACSPIOut(soundConfig.config.ssi.SSI, sounds[i].waveform[sounds[i].position]*200);
				}	
				/* TODO: this may not perform superposition of waves properly. */
				sounds[i].position = (sounds[i].position + 1) % MAX_SOUND_ELEM;
			}
		}
	}
}

/**
 * initializeSoundPlayer sets up the timer and configures the output.
 * @param soundConfig The output format of the audio. Requires calling 
 *					  EnableInterrupts() after initialization. 
 * @note Only one sound player can be activated at a time (i.e. only one set of
 *		 HW), but multiple channels can be played on the single set of hardware. 
 */
void initializeSoundPlayer(struct SoundConfig _soundConfig) {
	/* Player is initialized to 400 kHz (2500 ns). */
	TimerConfig_t timerConfig = {
		SYSTICK,
		freqToPeriod(400000, MAX_FREQ),
		true,
		2,
		iterateCycle
	};
	TimerInit(timerConfig);
	soundConfig = _soundConfig;

	/* Configure the DAC. */
	if (soundConfig.source == R_DAC) DACInit(soundConfig.config.pins);
	else DACSPIInit(soundConfig.config.ssi);
}

/**
 * playSound plays a tone at a specified frequency and envelope for the provided
 * pins. Utilizes timer 2 for interrupts.
 * @param id		Identifier of the sound being played. Up to 5 unique
 *					identifiers can be played. Useful for playing multiple
 *					sounds at once. -1 is an invalid ID.
 * @param freq		Frequency of the sound being played. Up to 2kHz.
 * @param waveform	Reference to a 32 entry waveform where each entry is a value
 *					from 0 to 256. Sound envelope.
 */
void playSound(int8_t id, uint32_t freq, uint8_t* waveform) {
	/* Exit early if the maximum number of sounds are already configured. */
	if (soundsConfigured == MAX_SOUNDS) return;    

	DisableInterrupts();
	/*
		440 hz * 16 subevents = 7040 Hz for each sound subevent.
		Equivalent to 11363 12.5 ns (80 MHz) iterations.
		Timer clock is 2500 ns (400 kHz), equivalent to 200 12.5 ns iterations.
		Number of iterations required is 11363 / 200 = 56.8.
		Max theoretical frequency is 
		
		  440 hz			 25 kHz
		 ---------	  =>	----------
		56.8 cycles 		 1 cycle

		Of course, we never get this far since the ISR is not ideally 1 80 MHz
		cycle, so as the sound frequency increases, the more likely we interrupt
		ourselves. The more things we do in the ISR, the more strict our bounds
		need to be.
	*/

	/* Apply the transformation function on the input. */
	freq = (freq * (1 + freq / 17500))/1.87;

	/* Convert freq into period for 1/16 notes. */
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
		sounds[foundIdx].position = 0;
	} 
	/* We did not find an existing tone, but we found an open slot to insert the
	 * sound. */
	else if (newIdx != -1) {
		sounds[newIdx].id = id;
		sounds[newIdx].periodSteps = period;
		sounds[newIdx].currentPeriodSteps = 0;
		sounds[newIdx].waveform = waveform;
		sounds[newIdx].position = 0;   
	}
	
	/* We didn't find an existing sound matching the ID nor an open spot. Do
	 * nothing. */
	EnableInterrupts();
}

/**
 * stopSound stops a tone from playing with a given id.
 * If the ID doesn't exist, do nothing.
 * @param id		Identifier of the sound being played.
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
			//	   sounds[i].pins.pinList[j] = PIN_COUNT;
			// }
			sounds[i].position = 0;
			--soundsConfigured;
			return;
		}
	}
}
