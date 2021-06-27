#ifndef QTRSensor_H
#define QTRSensor_H


#define MAX_ANALOG_PORTS 9
#define NUM_CONVERTERS 2

int ADC0 = 0;

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

/* Array containing the status of each analog port */
AnalogPortStatus_t ports[MAX_ANALOG_PORTS] = {UNINIT, UNINIT, UNINIT, UNINIT, UNINIT, UNINIT, UNINIT, UNINIT};


/* type that indicates whether there was an error or not */
typedef enum{
    ERROR = 1,
    NO_ERROR = 0
}Error_t;

/* Initializes ADC0 */
void ADC_Init(void);

/**
 *  Initialize Analog port
 * 
 * Input: Port name
 * 
 * Output: Error (1) if cannot initialize port due to ADC0 not being initialized before
 *  */

Error_t AnalogPort_Init(AnalogPort_t port);

/**
 * Read the data of an analog port
 * 
 * Inputs: desired analog port, pointer to variable you want the data to be stored in
 * 
 * Output: Error if port or ADC0 not initialized, NO_ERROR if everything worked as intented 
 *          In case of error, no data is passed into the "data parameter"         
 * */

Error_t ReadAnalogPort(AnalogPort_t port, uint16_t* data);

#endif 