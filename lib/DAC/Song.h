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

void stopMusic();