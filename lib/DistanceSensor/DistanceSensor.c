#include <lib/ADC/ADC.h>
#include <stdint.h>
#include <lib/DistanceSensor/DistanceSensor.h>
#include <lib/Timers/Timers.h>
#include <lib/GPIO/GPIO.h>

/* Holds the sensor for each of interrupt active */
static DistanceSensor_t* interruptSensors[MAX_ANALOG_PORTS];

/* Holds the interrupt configuration for each interrupt active */
static TimerConfig_t interruptConfig[MAX_ANALOG_PORTS];

/* Indicates if an interrupt is active (1) or not active (0). There's a total of 9 possible interrupts */
static uint8_t InterruptsActive[MAX_ANALOG_PORTS] = {0, 0, 0, 0, 0, 0, 0, 0, 0};

volatile int count1 = 0;

volatile int count2 = 0;

void DistanceSensor_Init(DistanceSensor_t* sensor, AnalogPort_t OUT_pin, GPIOConfig_t EN_pin){
    sensor->OUT = OUT_pin;
    sensor->EN = EN_pin;

    GPIOInit(sensor->EN);
    GPIOSetBit(sensor->EN.GPIOPin, 1);
    sensor->isEnabled = 1;

}

void DistanceSensor_Enable(DistanceSensor_t* sensor){

    if(sensor->isEnabled != 1){
    GPIOSetBit(sensor->EN.GPIOPin, 1);
    sensor->isEnabled = 1;
    }

}

void DistanceSensor_Disable(DistanceSensor_t* sensor){

    if(sensor->isEnabled != 0){
    GPIOSetBit(sensor->EN.GPIOPin, 0);
    sensor->isEnabled = 0;
    }

}

SensorStatus_t DistanceSensor_ShowStatus(DistanceSensor_t sensor){
    if(sensor.isEnabled == 1) return ENABLED;

    else return DISABLED;
}

void DistanceSensor_Read(DistanceSensor_t* sensor){
    ReadAnalogPort(sensor->OUT, &(sensor->data));
}

    /* Interrupt Handlers */

void DistanceSensor_Handler0(){
    DistanceSensor_Read(interruptSensors[0]);
}

void DistanceSensor_Handler1(){
    DistanceSensor_Read(interruptSensors[1]);
}

void DistanceSensor_Handler2(){
    DistanceSensor_Read(interruptSensors[2]);
}

void DistanceSensor_Handler3(){
    DistanceSensor_Read(interruptSensors[3]);
}

void DistanceSensor_Handler4(){
    DistanceSensor_Read(interruptSensors[4]);
}

void DistanceSensor_Handler5(){
    DistanceSensor_Read(interruptSensors[5]);
}

void DistanceSensor_Handler6(){
    DistanceSensor_Read(interruptSensors[6]);
}

void DistanceSensor_Handler7(){
    DistanceSensor_Read(interruptSensors[7]);
}

void DistanceSensor_Handler8(){
    DistanceSensor_Read(interruptSensors[8]);
}

void DistanceSensor_SetInterrupt(DistanceSensor_t* sensor, uint32_t frequency, TimerID_t timer, uint8_t priority){

    uint8_t interruptIndex;

    if(sensor->isInterrupt == 1) return;

    for(interruptIndex = 0; interruptIndex <= 8; interruptIndex++){
        if(InterruptsActive[interruptIndex] == 0) break;
    }

    if(interruptIndex <= 8){

        switch (interruptIndex)
        {
        case 0:
            
            interruptConfig[interruptIndex].timerID = timer;
            interruptConfig[interruptIndex].period = freqToPeriod(frequency, MAX_FREQ);
            interruptConfig[interruptIndex].isPeriodic = true;
            interruptConfig[interruptIndex].priority = priority;
            interruptConfig[interruptIndex].handlerTask = DistanceSensor_Handler0;

            sensor->isInterrupt = 1;
            sensor->interruptNum = interruptIndex;
            sensor->interruptConfig = &interruptConfig[interruptIndex];
            
            interruptSensors[interruptIndex] = sensor;
            

            InterruptsActive[interruptIndex] = 1;
            TimerInit(interruptConfig[interruptIndex]);
            break;

        case 1:
            
            interruptConfig[interruptIndex].timerID = timer;
            interruptConfig[interruptIndex].period = freqToPeriod(frequency, MAX_FREQ);
            interruptConfig[interruptIndex].isPeriodic = true;
            interruptConfig[interruptIndex].priority = priority;
            interruptConfig[interruptIndex].handlerTask = DistanceSensor_Handler1;

            sensor->isInterrupt = 1;
            sensor->interruptNum = interruptIndex;
            sensor->interruptConfig = &interruptConfig[interruptIndex];
            
            interruptSensors[interruptIndex] = sensor;
            

            InterruptsActive[interruptIndex] = 1;
            TimerInit(interruptConfig[interruptIndex]);
            break;

        case 2:

            interruptConfig[interruptIndex].timerID = timer;
            interruptConfig[interruptIndex].period = freqToPeriod(frequency, MAX_FREQ);
            interruptConfig[interruptIndex].isPeriodic = true;
            interruptConfig[interruptIndex].priority = priority;
            interruptConfig[interruptIndex].handlerTask = DistanceSensor_Handler2;

            sensor->isInterrupt = 1;
            sensor->interruptNum = interruptIndex;
            sensor->interruptConfig = &interruptConfig[interruptIndex];
            
            interruptSensors[interruptIndex] = sensor;
            

            InterruptsActive[interruptIndex] = 1;
            TimerInit(interruptConfig[interruptIndex]);
            break;

        case 3: 

            interruptConfig[interruptIndex].timerID = timer;
            interruptConfig[interruptIndex].period = freqToPeriod(frequency, MAX_FREQ);
            interruptConfig[interruptIndex].isPeriodic = true;
            interruptConfig[interruptIndex].priority = priority;
            interruptConfig[interruptIndex].handlerTask = DistanceSensor_Handler0;

            sensor->isInterrupt = 1;
            sensor->interruptNum = interruptIndex;
            sensor->interruptConfig = &interruptConfig[interruptIndex];
            
            interruptSensors[interruptIndex] = sensor;
            

            InterruptsActive[interruptIndex] = 1;
            TimerInit(interruptConfig[interruptIndex]);
            break;

        case 4:

            interruptConfig[interruptIndex].timerID = timer;
            interruptConfig[interruptIndex].period = freqToPeriod(frequency, MAX_FREQ);
            interruptConfig[interruptIndex].isPeriodic = true;
            interruptConfig[interruptIndex].priority = priority;
            interruptConfig[interruptIndex].handlerTask = DistanceSensor_Handler0;

            sensor->isInterrupt = 1;
            sensor->interruptNum = interruptIndex;
            sensor->interruptConfig = &interruptConfig[interruptIndex];
            
            interruptSensors[interruptIndex] = sensor;
            

            InterruptsActive[interruptIndex] = 1;
            TimerInit(interruptConfig[interruptIndex]);
            break;

        case 5:

            interruptConfig[interruptIndex].timerID = timer;
            interruptConfig[interruptIndex].period = freqToPeriod(frequency, MAX_FREQ);
            interruptConfig[interruptIndex].isPeriodic = true;
            interruptConfig[interruptIndex].priority = priority;
            interruptConfig[interruptIndex].handlerTask = DistanceSensor_Handler0;

            sensor->isInterrupt = 1;
            sensor->interruptNum = interruptIndex;
            sensor->interruptConfig = &interruptConfig[interruptIndex];
            
            interruptSensors[interruptIndex] = sensor;
            

            InterruptsActive[interruptIndex] = 1;
            TimerInit(interruptConfig[interruptIndex]);
            break;

        case 6:

            interruptConfig[interruptIndex].timerID = timer;
            interruptConfig[interruptIndex].period = freqToPeriod(frequency, MAX_FREQ);
            interruptConfig[interruptIndex].isPeriodic = true;
            interruptConfig[interruptIndex].priority = priority;
            interruptConfig[interruptIndex].handlerTask = DistanceSensor_Handler0;

            sensor->isInterrupt = 1;
            sensor->interruptNum = interruptIndex;
            sensor->interruptConfig = &interruptConfig[interruptIndex];
            
            interruptSensors[interruptIndex] = sensor;
            

            InterruptsActive[interruptIndex] = 1;
            TimerInit(interruptConfig[interruptIndex]);
            break;

        case 7:

            interruptConfig[interruptIndex].timerID = timer;
            interruptConfig[interruptIndex].period = freqToPeriod(frequency, MAX_FREQ);
            interruptConfig[interruptIndex].isPeriodic = true;
            interruptConfig[interruptIndex].priority = priority;
            interruptConfig[interruptIndex].handlerTask = DistanceSensor_Handler0;

            sensor->isInterrupt = 1;
            sensor->interruptNum = interruptIndex;
            sensor->interruptConfig = &interruptConfig[interruptIndex];
            
            interruptSensors[interruptIndex] = sensor;
            

            InterruptsActive[interruptIndex] = 1;
            TimerInit(interruptConfig[interruptIndex]);
            break;

        case 8:

            interruptConfig[interruptIndex].timerID = timer;
            interruptConfig[interruptIndex].period = freqToPeriod(frequency, MAX_FREQ);
            interruptConfig[interruptIndex].isPeriodic = true;
            interruptConfig[interruptIndex].priority = priority;
            interruptConfig[interruptIndex].handlerTask = DistanceSensor_Handler0;

            sensor->isInterrupt = 1;
            sensor->interruptNum = interruptIndex;
            sensor->interruptConfig = &interruptConfig[interruptIndex];
            
            interruptSensors[interruptIndex] = sensor;
            

            InterruptsActive[interruptIndex] = 1;
            TimerInit(interruptConfig[interruptIndex]);
            break;
			}
    }
}

void DistanceSensor_DisableInterrupt(DistanceSensor_t* sensor){

    if(sensor->isInterrupt == 0) return;

    
    TimerStop(sensor->interruptConfig->timerID);

    sensor->isInterrupt = 0;
    sensor->interruptConfig = 0;
		InterruptsActive[sensor->interruptNum] = 0;


}

