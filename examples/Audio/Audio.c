/**
 * Audio.c
 * Devices: LM4F120; TM4C123
 * Description: Middle level drivers to produce sounds in tandem with duration.
 * Authors: Matthew Yu & Gabby Williams 
 * Last Modified: 03/10/21
 **/
 
#include "../lib/DAC/Audio.h"
 
#define MAXLEN 100       //unsure of track length


Note_t trackQueue1[MAXLEN];
Note_t trackQueue2[MAXLEN];
Note_t trackQueue3[MAXLEN];

uint16_t Index = 0;
 
 /** 
 * playNote
 */
void playNote(Note_t t, Note_t track[]){
	
	if(isQueueEmpty(track) == true){     //is this necessary? 
		track[Index] = t;
		} else track[Index] = t;
	if(isQueueFull(track) == true){
//		playSound(track, MAX_FREQ, wave, config);   //fix this
	}
	Index++;
}

/**
 * trackqueue1 note1 note2 note3
 * trackqueue2 note4
 * trackqueue3 note5
 */
bool isQueueFull(Note_t track[]){
	
	if(Index > MAXLEN){
		return true; 
	} else return false;
	
	
}
bool isQueueEmpty(Note_t track[]){
	
	if(Index == 0){
		return true;
	} else return false; 

}

