/**
 * File name: DistanceSensor.h
 * Devices: TM4C123
 * Description: Low-level driver for GP2Y0A60SZ analog distance sensor
 * Authors: Dario Jimenez
 * Last Modified: 07/22/21
 * 
 * GP2Y0A60SZLF Analog Distance Sensor information:
 *    - 1 VCC pin: connect to the +3.3V pin or can be powered at 5V or 3V depending on the sensor
 *    - 1 GND pin
 *    - 1 OUT pin: ADC analog output with a range of 0V-3V at 5V VCC or 0V-1.6V at 3V VCC
 *    - 1 EN pin: optional pin to enable and disable sensor
 * 
 *    - Range of measuring distance: 10 cm to 150 cm (4'' to 60'')
 *    - update period: 16.5 ± 4 ms
 * 
 *    -Available TM4C123 pins for OUT pin: PE5, PE3, PE2, PE1, PE0, PD3, PD2, PD1, PD0
 * 
 * Summary of driver:
 *    - this driver allows you to initialize Distance sensors, read an unsigned 16-bit value from 
 *      the sensors, and set up periodic interrupts where data is read from a sensor at a desired 
 *      frequency.
 * 
 **/

#ifndef DistanceSensor_H
#define DistanceSensor_H

/* Standard C library import */
#include <stdint.h>

/* UTRASWARE library imports */
#include <lib/ADC/ADC.h>
#include <lib/GPIO/GPIO.h>
#include <lib/Timers/Timers.h>

/** 
 * Data structure for Distance Sensor
 * 
 * Only variable of importance for user is the "data" field, which contains the most recent value read from the sensor 
 * 
 * All other variables should not be modified by user or else sensor will not work properly.
 **/
typedef struct DistanceSensor{

    /* the Analog pin used for the sensor */
    AnalogPort_t OUT;

    /* boolean value indicating wheter the EN pin is used */
    uint8_t isUsingEN;

    /* The GPIO digital output pin for the enable pin */
    GPIOConfig_t EN; 

    /* boolean value that indicates if sensor is enabled */
    int8_t isEnabled;

    /* boolean value that indicates if there is an interrupt set up in sensor */
    uint8_t isInterrupt;

    /* index of where the configuration and information about the sensor's interrupt are stored */
    uint8_t interruptNum;

    /* configuration of the sensor's interrupt, if any interrupts are activated */
    TimerConfig_t* interruptConfig;

    /* stores the most recent data from the sensor */
    uint16_t data;

}DistanceSensor_t;

/* data type that indicates if a sensor is enabled or disabled */
typedef enum{
    DS_ENABLED,
    DS_DISABLED
}DistanceSensorStatus_t;

/**
 * Initialize a Distance Sensor without using the EN pin
 * 
 * Inputs: 1) DistanceSensor struct passed as a pointer
 *         2) Desired input analog pin for sensor (See ADC.h in lib/ADC/ for the full list of available pins)
 *              - pin must be initialized as input, analog on, alternative on, alternative function 8
 * 
 * 
 * Note: EN pin must be connected to a 3.3V power source or else the sensor won't be enabled
 * 
 *       You should connect a 10k ohm resistor between the 3.3V power source and the EN pin 
 *       or else the sensor and microcontroller could be damaged.
 * 
 * Output: no output
 **/

void DistanceSensor_Init(DistanceSensor_t* sensor, AnalogPort_t OUT_pin);

/**
 * Initialize a Distance Sensor using the Enable (EN) pin
 * 
 * Inputs: 1) DistanceSensor struct passed as a pointer
 *         2) Desired input analog pin for sensor (See ADC.h in lib/ADC/ for the full list of available pins)
 *              - pin must be initialized as input, analog on, alternative on, alternative function 8
 *         3) GPIO output pin for enable pin
 *              - pin must be initialized as digital output, analog off, alternate off, alternate function 0
 * 
 * Note: a 10k ohm resistor MUST be connected between EN pin and GPIO output pin or you could potentially damage the 
 *       sensor and microcontroller.
 *       
 * Output: no output
 **/
void DistanceSensor_ENInit(DistanceSensor_t* sensor, AnalogPort_t OUT_pin, GPIOConfig_t EN_pin);

/**
 * Enable Distance Sensor if disabled
 * 
 * Inputs: 1) DistanceSensor struct of desired sensor passed as a pointer
 * 
 * Output: no output
 **/
void DistanceSensor_Enable(DistanceSensor_t* sensor);

/**
 * Disable Distance Sensor
 * 
 * Inputs: 1) DistanceSensor struct of desired sensor passed as a pointer
 * 
 * Output: no output
 **/
void DistanceSensor_Disable(DistanceSensor_t* sensor);

/**
 * Show current status of Distance Sensor
 * 
 * Input: DistanceSensor struct passed by value (not a pointer)
 * 
 * Output: Status of sensor: 1) DS_ENABLED or 2) DS_DISABLED
 **/
DistanceSensorStatus_t DistanceSensor_ShowStatus(DistanceSensor_t sensor);

/**
 * Read analog value from Distance Sensor and store it in the data field of DistanceSensor struct
 * 
 * Inputs: DistanceSensor struct of desired sensor passed as a pointer
 * 
 * Output: not output, analog value is stored in the "data" variable of the struct
 * 
 * Note: to access the analog value read, you must access the "data" variable of the DistanceSensor struct
 *       It is accessed like such: if struct not a pointer: <struct Name>.data. If it is a pointer: <struct name>->data
 **/
void DistanceSensor_Read(DistanceSensor_t* sensor);

/** 
 * Initializes an interrupt to periodically read analog values from a given sensor at a desired frequency
 * 
 * 
 * Inputs: 1) DistanceSensor struct of desired sensor passed as a pointer
 *         2) The frequency at which the interrupt is going to be called or at which the sensor values will be read in Hz
 *         3) The desired timer for the interrupt (see Timers.h for a full list of available timers in lib/Timers)
 *         4) the desired priority of the interrupt from 0 to 7 (0 being the highest priority an 7 being the lowest)
 * 
 * Output: no output, data will be stored in the "data" variable from DistanceSensor struct at the given frequency
 * 
 * Note: In order to correctly read all values from the sensor, your interrupt frequency must be more than twice the
 *       frequency at which the sensor reads/updates its analog values
 *       i.e. if sensor updates its values at a frequency of 100 Hz, you must sample your interrupt at at least 201 Hz
 * 
 *      Update period of sensor: 16.5 ± 4 ms
 * 
 * Note: 9 different possible sensors can be set up as an interrupt at one time. If more sensors are tried to be set up as
 *       an interrupt, sensors outside of the first 9 will no be initialized as interrupts.
 *       
 * Note: In order to change the interrupt of a sensor that has already been set as an interrupt, you must disable the 
 *       interrupt first calling the "DistaceSensor_DisableInterrupt" function passing the desired sensor as parameter
 **/
void DistanceSensor_SetInterrupt(DistanceSensor_t* sensor, uint32_t frequency, TimerID_t timer, uint8_t priority);

/**
 * Disables the interrupt of a given Distance Sensor
 * 
 * Input: 1) DistanceSensor struct of desired sensor passed as a pointer
 * 
 * Output: no output
 **/
void DistanceSensor_DisableInterrupt(DistanceSensor_t* sensor);

#endif
