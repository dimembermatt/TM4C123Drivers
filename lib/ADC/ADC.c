/**
 * ADC.c
 * Devices: TM4C123
 * Description: Low-level drivers for ADC (Analog-to-digital converter) and analog ports
 * Authors: Dario Jimenez
 * Last Modified: 06/28/2021
 * 
 * This driver allows you to initialize ADC0 in the TM4C123 
 * microcontroller and utilize its 9 available analog ports/pins. 
 */

/* Header file import */
#include <lib/ADC/ADC.h> 

/* Device Import */
#include <inc/tm4c123gh6pm.h>

/* Standard Library Import */
#include <stdint.h>

/* Indicates status of ADCO; for driver use ONLY. DO NOT modify this value or you may experience errors */
static int ADC0 = 0;

/**
 * Initializes ADC0 
 * 
 * Inputs: no inputs
 * 
 * Outputs: no outputs
 * 
 * Call while interrupts are disabled
 */

void ADC_Init(void){
	volatile unsigned long delay;
  SYSCTL_RCGCADC_R |= 0x01;       // 6) activate ADC0 
  delay = SYSCTL_RCGCADC_R;       // extra time to stabilize
  delay = SYSCTL_RCGCADC_R;       // extra time to stabilize
  delay = SYSCTL_RCGCADC_R;       // extra time to stabilize
  delay = SYSCTL_RCGCADC_R;
  ADC0_PC_R = 0x01;               // 7) configure for 125K 
  ADC0_SSPRI_R = 0x0123;          // 8) Seq 3 is highest priority
  ADC0_ACTSS_R &= ~0x0008;        // 9) disable sample sequencer 3
  ADC0_EMUX_R &= ~0xF000;         // 10) seq3 is software trigger
 // ADC0_SSMUX3_R = (ADC0_SSMUX3_R&0xFFFFFFF0)+5;  // 11) Ain5 (PD2)
  ADC0_SSCTL3_R = 0x0006;         // 12) no TS0 D0, yes IE0 END0
  ADC0_IM_R &= ~0x0008;           // 13) disable SS3 interrupts
  ADC0_ACTSS_R |= 0x0008; 
  ADC0 = 1;                       // 14) set status of ADC0 to initialized
}


/**
 * Reads the data from an analog port
 * 
 * Inputs: desired analog port
 *         pointer to variable you want the data to be stored in
 * 
 * Output: ERROR (1) if port or ADC0 not initialized, which in case no valuable data is read
 *         NO_ERROR if everything worked as intented     
 * 
 * Note: if a pin that is not initialized to analog is read, erroneous data will be read
 *  
 **/

Error_t ReadAnalogPort(AnalogPort_t port, uint16_t* data){

    //Error if ADC0 not initialized
    if(ADC0 == 0) return ERROR;

    ADC0_SSMUX3_R = (ADC0_SSMUX3_R&0xFFFFFFF0)+port; //set the ADC0 channel to the desired port
    ADC0_PSSI_R = 0x0008; //start ADC
	while((ADC0_RIS_R & 0x08) == 0) {}; //Busy wait. If it's ready flag becomes 0
	*data = ADC0_SSFIFO3_R&0xFFF; //read Data
	ADC0_ISC_R = 0x0008; //clear Flag

	return NO_ERROR;
}

