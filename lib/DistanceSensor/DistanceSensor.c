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

/* Standard C library import */
#include <stdint.h>

/* UTRASWARE library imports */
#include <lib/DistanceSensor/DistanceSensor.h>
#include <lib/ADC/ADC.h>
#include <lib/Timers/Timers.h>
#include <lib/GPIO/GPIO.h>

/* Holds the sensor for each of interrupt active */
static DistanceSensor_t* interruptSensors[MAX_ANALOG_PORTS];

/* Holds the interrupt configuration for each interrupt active */
static TimerConfig_t interruptConfig[MAX_ANALOG_PORTS];

/* Indicates if an interrupt is active (1) or not active (0). There's a total of 9 possible interrupts */
static uint8_t InterruptsActive[MAX_ANALOG_PORTS] = {0, 0, 0, 0, 0, 0, 0, 0, 0};


/**
 * Initialize a Distance Sensor without using the EN pin
 * 
 * Inputs: 1) DistanceSensor struct passed as a pointer
 *         2) Desired input analog pin for sensor (See ADC.h in lib/ADC/ for the full list of available pins)
 *              - pin must be initialized as input, analog on, alternative on, alternative function 8
 * 
 * 
 * Note: EN pin must be connected to a 3.3V power source or else the sensor won't be enabled
 *          - You should connect a 10k ohm resistor between the 3.3V power source and the EN pin 
 *            or else the sensor and microcontroller could be damaged.
 * 
 * Note: You won't be able to use functions "DistanceSensor_Enable" or "DistanceSensor_Disable" and hence
 *       won't be able to disable or enable the sensor at will; the sensor will always be enabled.
 * 
 * Output: no output
 **/

void DistanceSensor_Init(DistanceSensor_t* sensor, AnalogPort_t OUT_pin){

    /* stores the analog pin in struct */
    sensor->OUT = OUT_pin;

    /* Sets the Enable pin as high (3.3V or 1) to enable the sensor */
    GPIOSetBit(sensor->EN.GPIOPin, 1);

    /* store the current status of the sensor in struct */
    sensor->isEnabled = 1;

    /* Indicate that Enable pin is not being used */
    sensor->isUsingEN = 0;
}

/**
 * Initialize a Distance Sensor
 * 
 * Inputs: 1) DistanceSensor struct passed as a pointer
 *         2) Desired input analog pin for sensor (See ADC.h in lib/ADC/ for the full list of available pins)
 *              - pin must be initialized as input, analog on, alternative on, alternative function 8
 *         3) GPIO output pin for enable pin
 *              - pin must be initialized as digital output, analog off, alternate off, alternate function 0
 * 
 * Output: no output
 **/
void DistanceSensor_ENInit(DistanceSensor_t* sensor, AnalogPort_t OUT_pin, GPIOConfig_t EN_pin){

    /* stores the analog pin in struct */
    sensor->OUT = OUT_pin;

    /* stores the Enable pin in struct */
    sensor->EN = EN_pin;

    /* Initializes the Enable pin */
    //GPIOInit(sensor->EN);

    /* Sets the Enable pin as high (3.3V or 1) to enable the sensor */
    GPIOSetBit(sensor->EN.GPIOPin, 1);

    /* store the current status of the sensor in struct */
    sensor->isEnabled = 1;

    /* Indicate that Enable pin is being used */
    sensor->isUsingEN = 1;

}
/**
 * Enable Distance Sensor if disabled
 * 
 * Inputs: 1) DistanceSensor struct of desired sensor passed as a pointer
 * 
 * Output: no output
 **/
void DistanceSensor_Enable(DistanceSensor_t* sensor){

    /* enable sensor only if it is disabled and EN pin is being used */
    if(sensor->isEnabled != 1 && sensor->isUsingEN == 1){
    
    /* set the pin output to 1 (high, 3.3v) to enable sensor */
    GPIOSetBit(sensor->EN.GPIOPin, 1);

    /* Indicate sensor is enabled in struct */
    sensor->isEnabled = 1;
    }

}
/**
 * Disable Distance Sensor
 * 
 * Inputs: 1) DistanceSensor struct of desired sensor passed as a pointer
 * 
 * Output: no output
 **/
void DistanceSensor_Disable(DistanceSensor_t* sensor){

    /* Disable sensoe only if it is enabled and EN pin is being used */
    if(sensor->isEnabled != 0 && sensor->isUsingEN == 1){

    /* Disable sensor by setting pin output to 0 (low, 0V) */
    GPIOSetBit(sensor->EN.GPIOPin, 0);

    /* Indicate sensor is not enabled in struct */
    sensor->isEnabled = 0;
    }

}

/**
 * Show current status of Distance Sensor
 * 
 * Input: DistanceSensor struct passed by value (not a pointer)
 * 
 * Output: Status of sensor: 1) DS_ENABLED or 2) DS_DISABLED
 **/
DistanceSensorStatus_t DistanceSensor_ShowStatus(DistanceSensor_t sensor){

    /* check struct's "isEnabled" boolean variable to check if sensor is enabled or disabled */
    if(sensor.isEnabled == 1) return DS_ENABLED; // 1 = enabled

    else return DS_DISABLED; // 0 = disabled
}

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
void DistanceSensor_Read(DistanceSensor_t* sensor){

    /* read analog value from OUT pin using the ADC */
    ReadAnalogPort(sensor->OUT, &(sensor->data));
}

/* Interrupt Handlers for each of the 9 possible interrupts */

void DistanceSensor_Handler0(){

    /* Reads the analog value from the OUT pin of the sensor in index 0 */
    DistanceSensor_Read(interruptSensors[0]);
}

void DistanceSensor_Handler1(){

    /* Reads the analog value from the OUT pin of the sensor in index 1 */
    DistanceSensor_Read(interruptSensors[1]);
}

void DistanceSensor_Handler2(){

    /* Reads the analog value from the OUT pin ofthe sensor in index 2 */
    DistanceSensor_Read(interruptSensors[2]);
}

void DistanceSensor_Handler3(){

    /* Reads the analog value from the OUT pin of the sensor in index 3 */
    DistanceSensor_Read(interruptSensors[3]);
}

void DistanceSensor_Handler4(){

    /* Reads the analog value from the OUT pin of the sensor in index 4 */
    DistanceSensor_Read(interruptSensors[4]);
}

void DistanceSensor_Handler5(){

    /* Reads the analog value from the OUT pin the sensor in index 5 */
    DistanceSensor_Read(interruptSensors[5]);
}

void DistanceSensor_Handler6(){

    /* Reads the analog value from the OUT pin of the sensor in index 6 */
    DistanceSensor_Read(interruptSensors[6]);
}

void DistanceSensor_Handler7(){

    /* Reads the analog value from the OUT pin the sensor in index 7 */
    DistanceSensor_Read(interruptSensors[7]);
}

void DistanceSensor_Handler8(){

    /* Reads the analog value from the OUT pin of the sensor in index 8 */
    DistanceSensor_Read(interruptSensors[8]);
}


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
void DistanceSensor_SetInterrupt(DistanceSensor_t* sensor, uint32_t frequency, TimerID_t timer, uint8_t priority){

    /* Index where the sensor and interrupt info will be stored */
    uint8_t interruptIndex;

    /* return and do nothing if sensor is already set up as an interrupt */
    if(sensor->isInterrupt == 1) return;

    /* searches through the array for an available spot to store the sensor and interrupt info */
    for(interruptIndex = 0; interruptIndex <= 8; interruptIndex++){
        if(InterruptsActive[interruptIndex] == 0) break;
    }

    /* the limit of interrupts has not been reached yet */
    if(interruptIndex <= 8){

        switch (interruptIndex)
        {

        /* configuration for sensors and interrupts */
        case 0:
            
            /* interrupt config */
            interruptConfig[interruptIndex].timerID = timer;
            interruptConfig[interruptIndex].period = freqToPeriod(frequency, MAX_FREQ);
            interruptConfig[interruptIndex].isPeriodic = true;
            interruptConfig[interruptIndex].priority = priority;
            interruptConfig[interruptIndex].handlerTask = DistanceSensor_Handler0; 

            /* sensor config */
            sensor->isInterrupt = 1; //indicate sensor has been set up as an interrupt
            sensor->interruptNum = interruptIndex; //store the index where sensor and interrupt info is stored
            sensor->interruptConfig = &interruptConfig[interruptIndex]; //store the interrupt config
            
            /* store the sensor struct */
            interruptSensors[interruptIndex] = sensor; 
            
            /* indicate that interrupt at this index is active */
            InterruptsActive[interruptIndex] = 1;

            /* initialize interrupt */
            TimerInit(interruptConfig[interruptIndex]);
            
            break;

        case 1:
            
             /* interrupt config */
            interruptConfig[interruptIndex].timerID = timer;
            interruptConfig[interruptIndex].period = freqToPeriod(frequency, MAX_FREQ);
            interruptConfig[interruptIndex].isPeriodic = true;
            interruptConfig[interruptIndex].priority = priority;
            interruptConfig[interruptIndex].handlerTask = DistanceSensor_Handler1; 

            /* sensor config */
            sensor->isInterrupt = 1; //indicate sensor has been set up as an interrupt
            sensor->interruptNum = interruptIndex; //store the index where sensor and interrupt info is stored
            sensor->interruptConfig = &interruptConfig[interruptIndex]; //store the interrupt config
            
            /* store the sensor struct */
            interruptSensors[interruptIndex] = sensor; 
            
            /* indicate that interrupt at this index is active */
            InterruptsActive[interruptIndex] = 1;

            /* initialize interrupt */
            TimerInit(interruptConfig[interruptIndex]);
            
            break;

        case 2:

             /* interrupt config */
            interruptConfig[interruptIndex].timerID = timer;
            interruptConfig[interruptIndex].period = freqToPeriod(frequency, MAX_FREQ);
            interruptConfig[interruptIndex].isPeriodic = true;
            interruptConfig[interruptIndex].priority = priority;
            interruptConfig[interruptIndex].handlerTask = DistanceSensor_Handler2; 

            /* sensor config */
            sensor->isInterrupt = 1; //indicate sensor has been set up as an interrupt
            sensor->interruptNum = interruptIndex; //store the index where sensor and interrupt info is stored
            sensor->interruptConfig = &interruptConfig[interruptIndex]; //store the interrupt config
            
            /* store the sensor struct */
            interruptSensors[interruptIndex] = sensor; 
            
            /* indicate that interrupt at this index is active */
            InterruptsActive[interruptIndex] = 1;

            /* initialize interrupt */
            TimerInit(interruptConfig[interruptIndex]);
            
            break;

        case 3: 

             /* interrupt config */
            interruptConfig[interruptIndex].timerID = timer;
            interruptConfig[interruptIndex].period = freqToPeriod(frequency, MAX_FREQ);
            interruptConfig[interruptIndex].isPeriodic = true;
            interruptConfig[interruptIndex].priority = priority;
            interruptConfig[interruptIndex].handlerTask = DistanceSensor_Handler3; 

            /* sensor config */
            sensor->isInterrupt = 1; //indicate sensor has been set up as an interrupt
            sensor->interruptNum = interruptIndex; //store the index where sensor and interrupt info is stored
            sensor->interruptConfig = &interruptConfig[interruptIndex]; //store the interrupt config
            
            /* store the sensor struct */
            interruptSensors[interruptIndex] = sensor; 
            
            /* indicate that interrupt at this index is active */
            InterruptsActive[interruptIndex] = 1;

            /* initialize interrupt */
            TimerInit(interruptConfig[interruptIndex]);
            
            break;

        case 4:

             /* interrupt config */
            interruptConfig[interruptIndex].timerID = timer;
            interruptConfig[interruptIndex].period = freqToPeriod(frequency, MAX_FREQ);
            interruptConfig[interruptIndex].isPeriodic = true;
            interruptConfig[interruptIndex].priority = priority;
            interruptConfig[interruptIndex].handlerTask = DistanceSensor_Handler4; 

            /* sensor config */
            sensor->isInterrupt = 1; //indicate sensor has been set up as an interrupt
            sensor->interruptNum = interruptIndex; //store the index where sensor and interrupt info is stored
            sensor->interruptConfig = &interruptConfig[interruptIndex]; //store the interrupt config
            
            /* store the sensor struct */
            interruptSensors[interruptIndex] = sensor; 
            
            /* indicate that interrupt at this index is active */
            InterruptsActive[interruptIndex] = 1;

            /* initialize interrupt */
            TimerInit(interruptConfig[interruptIndex]);
            
            break;

        case 5:

             /* interrupt config */
            interruptConfig[interruptIndex].timerID = timer;
            interruptConfig[interruptIndex].period = freqToPeriod(frequency, MAX_FREQ);
            interruptConfig[interruptIndex].isPeriodic = true;
            interruptConfig[interruptIndex].priority = priority;
            interruptConfig[interruptIndex].handlerTask = DistanceSensor_Handler0; 

            /* sensor config */
            sensor->isInterrupt = 1; //indicate sensor has been set up as an interrupt
            sensor->interruptNum = interruptIndex; //store the index where sensor and interrupt info is stored
            sensor->interruptConfig = &interruptConfig[interruptIndex]; //store the interrupt config
            
            /* store the sensor struct */
            interruptSensors[interruptIndex] = sensor; 
            
            /* indicate that interrupt at this index is active */
            InterruptsActive[interruptIndex] = 1;

            /* initialize interrupt */
            TimerInit(interruptConfig[interruptIndex]);
            
            break;

        case 6:

             /* interrupt config */
            interruptConfig[interruptIndex].timerID = timer;
            interruptConfig[interruptIndex].period = freqToPeriod(frequency, MAX_FREQ);
            interruptConfig[interruptIndex].isPeriodic = true;
            interruptConfig[interruptIndex].priority = priority;
            interruptConfig[interruptIndex].handlerTask = DistanceSensor_Handler0; 

            /* sensor config */
            sensor->isInterrupt = 1; //indicate sensor has been set up as an interrupt
            sensor->interruptNum = interruptIndex; //store the index where sensor and interrupt info is stored
            sensor->interruptConfig = &interruptConfig[interruptIndex]; //store the interrupt config
            
            /* store the sensor struct */
            interruptSensors[interruptIndex] = sensor; 
            
            /* indicate that interrupt at this index is active */
            InterruptsActive[interruptIndex] = 1;

            /* initialize interrupt */
            TimerInit(interruptConfig[interruptIndex]);
            
            break;

        case 7:

             /* interrupt config */
            interruptConfig[interruptIndex].timerID = timer;
            interruptConfig[interruptIndex].period = freqToPeriod(frequency, MAX_FREQ);
            interruptConfig[interruptIndex].isPeriodic = true;
            interruptConfig[interruptIndex].priority = priority;
            interruptConfig[interruptIndex].handlerTask = DistanceSensor_Handler0; 

            /* sensor config */
            sensor->isInterrupt = 1; //indicate sensor has been set up as an interrupt
            sensor->interruptNum = interruptIndex; //store the index where sensor and interrupt info is stored
            sensor->interruptConfig = &interruptConfig[interruptIndex]; //store the interrupt config
            
            /* store the sensor struct */
            interruptSensors[interruptIndex] = sensor; 
            
            /* indicate that interrupt at this index is active */
            InterruptsActive[interruptIndex] = 1;

            /* initialize interrupt */
            TimerInit(interruptConfig[interruptIndex]);
            
            break;

        case 8:

             /* interrupt config */
            interruptConfig[interruptIndex].timerID = timer;
            interruptConfig[interruptIndex].period = freqToPeriod(frequency, MAX_FREQ);
            interruptConfig[interruptIndex].isPeriodic = true;
            interruptConfig[interruptIndex].priority = priority;
            interruptConfig[interruptIndex].handlerTask = DistanceSensor_Handler0; 

            /* sensor config */
            sensor->isInterrupt = 1; //indicate sensor has been set up as an interrupt
            sensor->interruptNum = interruptIndex; //store the index where sensor and interrupt info is stored
            sensor->interruptConfig = &interruptConfig[interruptIndex]; //store the interrupt config
            
            /* store the sensor struct */
            interruptSensors[interruptIndex] = sensor; 
            
            /* indicate that interrupt at this index is active */
            InterruptsActive[interruptIndex] = 1;

            /* initialize interrupt */
            TimerInit(interruptConfig[interruptIndex]);
            
            break;
			}
    }
}

/**
 * Disables the interrupt of a given Distance Sensor
 * 
 * Input: 1) DistanceSensor struct of desired sensor passed as a pointer
 * 
 * Output: no output
 **/
void DistanceSensor_DisableInterrupt(DistanceSensor_t* sensor){

    /* do nothing if sensor does not have an interrupt active */
    if(sensor->isInterrupt == 0) return;

    /* stop the interrupt */
    TimerStop(sensor->interruptConfig->timerID);

    /* indicate there is no longer an interrupt active for the sensor */
    sensor->isInterrupt = 0;

    /* reset the interrupt config */
    sensor->interruptConfig = 0;

    /* indicate there is no longer an interrupt active at the sensor's given index */
	InterruptsActive[sensor->interruptNum] = 0;


}

