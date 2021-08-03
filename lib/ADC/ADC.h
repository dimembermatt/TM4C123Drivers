/**
 * ADC.h
 * Devices: TM4C123
 * Description: Low-level drivers for ADC (Analog-to-digital converter) and analog ports
 * Authors: Dario Jimenez
 * Last Modified: 06/28/2021
 * 
 * This driver allows you to initialize ADC0 in the TM4C123 
 * microcontroller and utilize its 9 available analog ports/pins. 
 */

#ifndef QTRSensor_H
#define QTRSensor_H

/* libraries/imports used for this driver */
#include <stdint.h>

/* Macros or constant values */
#define MAX_ANALOG_PORTS 9


/* Available ports for ADC */
typedef enum{
    PE3 = 0,
    PE2 = 1,
    PE1 = 2,
    PE0 = 3,
    PD3 = 4,
    PD2 = 5,
    PD1 = 6,
    PD0 = 7,
    PE5 = 8
}AnalogPort_t;

/* Indicates the status of a port */
typedef enum{
    INIT, //port initialized
    UNINIT //port uninitialized
}AnalogPortStatus_t;


/* indicates whether there was an error or not */
typedef enum{
    ERROR = 1,
    NO_ERROR = 0
}Error_t;

/**
 * Initializes ADC0 
 * 
 * Inputs: no inputs
 * 
 * Outputs: no outputs
 * 
 * Call while interrupts are disabled
 */

void ADC_Init(void);


/**
 * Reads the data from an analog port
 * 
 * Inputs: desired analog port
 *         pointer to variable you want the data to be stored in
 * 
 * Output: ERROR (1) if port or ADC0 not initialized, which in case no valuable data is read
 *         NO_ERROR if everything worked as intented      
 * */

Error_t ReadAnalogPort(AnalogPort_t port, uint16_t* data);

#endif 
