/**
 * DAC.h
 * configures DAC output
 * runs on LM4F120 or TM4C123
 * @author: Matthew Yu
 * Last Modified: 11/28/2018
 * 11/28/18 - standardized comment style, preparing for c++ rewrite
 **/
#include <stdint.h>


/**
 * DAC_Init - Initialize 6-bit DAC, called once
 **/
void DAC_Init(void);


/**
 * DAC_Out - output to DAC
 * @param - data (6-bit data, 0 to 63)
 * //data_out = data*3.3V/63
 * data written to PortB data register
 **/
void DAC_Out(uint8_t data);
