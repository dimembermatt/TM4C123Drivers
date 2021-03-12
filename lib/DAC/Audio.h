/**
 * Audio.h
 * Devices: LM4F120; TM4C123
 * Description: Middle level drivers to produce sounds in tandem with duration.
 * Authors: Matthew Yu.
 * Last Modified: 03/10/21
 **/
#pragma once

/** General imports. */
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

/** Device specific imports. */
#include "Sound.h"

/** TODO: convert into frequency instead of period. */
#define C1	19112
#define Cs1	18040
#define Df1	18040
#define D1	17028
#define Ds1	16072
#define Ef1	16072
#define E1	15168
#define F1	14316
#define Fs1	13512
#define Gf1	13512
#define G1	12756
#define Gs1	12040
#define Af1	1204
#define A1	11364
#define As1	10724
#define Bf1	10724
#define B1	10124

#define C2	9556
#define Cs2	9020
#define Df2	9020
#define D2	8514
#define Ds2	8036
#define Ef2	8036
#define E2	7584
#define F2	7158
#define Fs2	6756
#define Gf2	6756
#define G2	6378
#define Gs2	6020
#define Af2	6020
#define A2	5682
#define As2	5362
#define Bf2	5362
#define B2	5062

#define C3	4778
#define Cs3	4510
#define Df3	4510
#define D3	4257
#define Ds3	4018
#define Ef3	4018
#define E3	3792
#define F3	3579
#define Fs3	3378
#define Gf3	3378
#define G3	3189
#define Gs3	3010
#define Af3	3010
#define A3	2841
#define As3	2681
#define Bf3	2681
#define B3	2531

#define C4	262 /* Done for you. */
#define Cs4	2255
#define Df4	2255
#define D4	2129
#define Ds4	2009
#define Ef4	2009
#define E4	1896
#define F4	1790
#define Fs4	1689
#define Gf4	1689
#define G4	1595
#define Gs4	1505
#define Af4	1505
#define A4	1421
#define As4	1341
#define Bf4	1341
#define B4	1266

#define C5	1195

enum NoteLength { 
    NONE, 
    sixteenth, 
    eighth, 
    quarter, 
    half, 
    whole
};

typedef struct Note {
    uint32_t frequency;
    uint16_t waveform;
    enum NoteLength length;
} Note_t;

/** 
 * playNote
 */
void playNote(Note_t t, uint8_t track);

/**
 * trackqueue1 note1 note2 note3
 * trackqueue2 note4
 * trackqueue3 note5
 */
bool isQueueFull(uint8_t track);
bool isQueueEmpty(uint8_t track);