/**
 * DAC.c
 * configures DAC output
 * runs on LM4F120 or TM4C123
 * Author: Matthew Yu
 * Last Modified: 11/28/2018
 * 11/28/18 - standardized comment style, preparing for c++ rewrite
 **/

#include <stdint.h>
#include "../inc/tm4c123gh6pm.h"


/**
 * DAC_Init - Initialize 6-bit DAC, called once
 **/
void DAC_Init(void){
	SYSCTL_RCGCGPIO_R |= 0x00000002;	//init Port B
	volatile int delay = 0;
	GPIO_PORTB_DIR_R |= 0x3F; //00111111 PB0-5
	GPIO_PORTB_AMSEL_R &= ~0x3F;
	GPIO_PORTB_DEN_R |= 0x3F;
}

/**
 * DAC_Out - output to DAC
 * @param - data (6-bit data, 0 to 63)
 * //data_out = data*3.3V/63
 * data written to PortB data register
 **/
void DAC_Out(uint8_t data){
	GPIO_PORTB_DATA_R = data;
}
