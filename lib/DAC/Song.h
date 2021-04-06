/**
 * Song.h
 * Devices: LM4F120; TM4C123
 * Description: High level drivers to play music.
 * Authors: Matthew Yu.
 * Last Modified: 03/11/21
 **/
#pragma once

/** General imports. */
#include <stdint.h>
#include <stdio.h>

/** Device specific imports. */
#include "Audio.h"


enum Volume {
	NONE,
	LOW,
	MEDIUM,
	HIGH
};

enum Speed {
	PAUSE,
	SLOW,
	NORMAL, // 120 BPM
	FAST
};

typedef struct Song {
	char *name;
	Note_t *melody;
	Note_t *harmony;
	enum Volume volume;
	enum Speed speed;
} Song_t;

void playMusic(Song_t song);

void stopMusic(); /* Stop the interrupt handler songHandler, and then call the clearQueues */

song
	primary [A] B C D]
	secondary [A] C D]

songHandler {
	// Do I have any notes left to play?
	// If yes 
		// Try to fill the queue for all tracks in the song
		while (!isQueueFull(primary))
			playNote(primary, nextNote);

		while (!isQueueFull(secondary))
			playNote(secondary, nextNote);
}

