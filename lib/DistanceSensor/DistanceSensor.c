#include <lib/ADC/ADC.h>
#include <stdint.h>
#include <lib/DistanceSensor/DistanceSensor.h>
#include <lib/Timers/Timers.h>

<<<<<<< HEAD
//TODO: ADD a "initialized" field to DistanceSensor_t to check if not initialize (Maybe)

DistanceSensor_t* interruptSensors[MAX_ANALOG_PORTS];
static TimerConfig_t* interruptConfig[MAX_ANALOG_PORTS];
static uint8_t interruptsActivated[MAX_ANALOG_PORTS];


void DistanceSensor_Init(DistanceSensor_t* sensor, AnalogPort_t OUT_pin, GPIOConfig_t EN_pin){
    sensor->OUT = OUT_pin;
    sensor->EN = EN_pin;

    GPIOInit(sensor->EN);
    GPIOSetBit(sensor->EN.GPIOPin, 1);
    sensor->isEnabled = 1;
    sensor->isInterrupt = 0;

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
    ReadAnalogPort(sensor->OUT, (sensor->data));
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

void DistanceSensor_SetInterrupt(DistanceSensor_t* sensor, uint16_t freq, uint8_t priority, TimerID_t timer){
    
    uint8_t interruptIndex;

    if(sensor->isInterrupt == 1) return;
    
    for(interruptIndex = 0; interruptIndex <= 8; interruptIndex++){
        if(interruptsActivated[interruptIndex] == 0) break;
    }

    if(interruptIndex <= 8){

        interruptSensors[interruptIndex] = sensor;

        switch (interruptIndex)
        {
        case 0:

            interruptConfig[interruptIndex]->timerID = timer;
            interruptConfig[interruptIndex]->period = freqToPeriod(freq, MAX_FREQ);
            interruptConfig[interruptIndex]->isPeriodic = true;
            interruptConfig[interruptIndex]->priority = priority;
            interruptConfig[interruptIndex]->handlerTask = DistanceSensor_Handler0;

            interruptSensors[interruptIndex]->isInterrupt = 1;
            interruptSensors[interruptIndex]->interruptNum = interruptIndex;
            interruptSensors[interruptIndex]->interruptConfig = interruptConfig[interruptIndex];

            interruptsActivated[interruptIndex] = 1;
            TimerInit(*(interruptConfig[interruptIndex]));
            break;

        
        case 1:
            interruptConfig[interruptIndex]->timerID = timer;
            interruptConfig[interruptIndex]->period = freqToPeriod(freq, MAX_FREQ);
            interruptConfig[interruptIndex]->isPeriodic = true;
            interruptConfig[interruptIndex]->priority = priority;
            interruptConfig[interruptIndex]->handlerTask = DistanceSensor_Handler1;

            interruptSensors[interruptIndex]->isInterrupt = 1;
            interruptSensors[interruptIndex]->interruptNum = interruptIndex;
            interruptSensors[interruptIndex]->interruptConfig = interruptConfig[interruptIndex];

            interruptsActivated[interruptIndex] = 1;
            TimerInit(*(interruptConfig[interruptIndex]));
            break;

        case 2:
            interruptConfig[interruptIndex]->timerID = timer;
            interruptConfig[interruptIndex]->period = freqToPeriod(freq, MAX_FREQ);
            interruptConfig[interruptIndex]->isPeriodic = true;
            interruptConfig[interruptIndex]->priority = priority;
            interruptConfig[interruptIndex]->handlerTask = DistanceSensor_Handler2;

            interruptSensors[interruptIndex]->isInterrupt = 1;
            interruptSensors[interruptIndex]->interruptNum = interruptIndex;
            interruptSensors[interruptIndex]->interruptConfig = interruptConfig[interruptIndex];

            interruptsActivated[interruptIndex] = 1;
            TimerInit(*(interruptConfig[interruptIndex]));
            break;

        case 3:
            interruptConfig[interruptIndex]->timerID = timer;
            interruptConfig[interruptIndex]->period = freqToPeriod(freq, MAX_FREQ);
            interruptConfig[interruptIndex]->isPeriodic = true;
            interruptConfig[interruptIndex]->priority = priority;
            interruptConfig[interruptIndex]->handlerTask = DistanceSensor_Handler3;

            interruptSensors[interruptIndex]->isInterrupt = 1;
            interruptSensors[interruptIndex]->interruptNum = interruptIndex;
            interruptSensors[interruptIndex]->interruptConfig = interruptConfig[interruptIndex];

            interruptsActivated[interruptIndex] = 1;
            TimerInit(*(interruptConfig[interruptIndex]));
            break;

        case 4:
            interruptConfig[interruptIndex]->timerID = timer;
            interruptConfig[interruptIndex]->period = freqToPeriod(freq, MAX_FREQ);
            interruptConfig[interruptIndex]->isPeriodic = true;
            interruptConfig[interruptIndex]->priority = priority;
            interruptConfig[interruptIndex]->handlerTask = DistanceSensor_Handler4;

            interruptSensors[interruptIndex]->isInterrupt = 1;
            interruptSensors[interruptIndex]->interruptNum = interruptIndex;
            interruptSensors[interruptIndex]->interruptConfig = interruptConfig[interruptIndex];

            interruptsActivated[interruptIndex] = 1;
            TimerInit(*(interruptConfig[interruptIndex]));
            break;

        case 5:
            interruptConfig[interruptIndex]->timerID = timer;
            interruptConfig[interruptIndex]->period = freqToPeriod(freq, MAX_FREQ);
            interruptConfig[interruptIndex]->isPeriodic = true;
            interruptConfig[interruptIndex]->priority = priority;
            interruptConfig[interruptIndex]->handlerTask = DistanceSensor_Handler5;

            interruptSensors[interruptIndex]->isInterrupt = 1;
            interruptSensors[interruptIndex]->interruptNum = interruptIndex;
            interruptSensors[interruptIndex]->interruptConfig = interruptConfig[interruptIndex];

            interruptsActivated[interruptIndex] = 1;
            TimerInit(*(interruptConfig[interruptIndex]));
            break;
        
        case 6:
            interruptConfig[interruptIndex]->timerID = timer;
            interruptConfig[interruptIndex]->period = freqToPeriod(freq, MAX_FREQ);
            interruptConfig[interruptIndex]->isPeriodic = true;
            interruptConfig[interruptIndex]->priority = priority;
            interruptConfig[interruptIndex]->handlerTask = DistanceSensor_Handler6;

            interruptSensors[interruptIndex]->isInterrupt = 1;
            interruptSensors[interruptIndex]->interruptNum = interruptIndex;
            interruptSensors[interruptIndex]->interruptConfig = interruptConfig[interruptIndex];

            interruptsActivated[interruptIndex] = 1;
            TimerInit(*(interruptConfig[interruptIndex]));
            break;

        case 7:
            interruptConfig[interruptIndex]->timerID = timer;
            interruptConfig[interruptIndex]->period = freqToPeriod(freq, MAX_FREQ);
            interruptConfig[interruptIndex]->isPeriodic = true;
            interruptConfig[interruptIndex]->priority = priority;
            interruptConfig[interruptIndex]->handlerTask = DistanceSensor_Handler7;

            interruptSensors[interruptIndex]->isInterrupt = 1;
            interruptSensors[interruptIndex]->interruptNum = interruptIndex;
            interruptSensors[interruptIndex]->interruptConfig = interruptConfig[interruptIndex];

            interruptsActivated[interruptIndex] = 1;
            TimerInit(*(interruptConfig[interruptIndex]));
            break;

        case 8:
            interruptConfig[interruptIndex]->timerID = timer;
            interruptConfig[interruptIndex]->period = freqToPeriod(freq, MAX_FREQ);
            interruptConfig[interruptIndex]->isPeriodic = true;
            interruptConfig[interruptIndex]->priority = priority;
            interruptConfig[interruptIndex]->handlerTask = DistanceSensor_Handler8;

            interruptSensors[interruptIndex]->isInterrupt = 1;
            interruptSensors[interruptIndex]->interruptNum = interruptIndex;
            interruptSensors[interruptIndex]->interruptConfig = interruptConfig[interruptIndex];

            interruptsActivated[interruptIndex] = 1;
            TimerInit(*(interruptConfig[interruptIndex]));
            break;
        }
    }
=======
void DistanceSensor_Init(DistanceSensor_t* sensor, AnalogPort_t pin){
    sensor->pin = pin;
}

void DistanceSensor_Read(DistanceSensor_t* sensor){
    ReadAnalogPort(sensor->pin, &(sensor->data));
>>>>>>> parent of 31f778b (Added/modified methods)
}

void DistanceSensor_DisableInterrupt(DistanceSensor_t* sensor){

    if(sensor->isInterrupt == 1){
        sensor->isInterrupt = 0;
        TimerStop(sensor->interruptConfig->timerID);
        interruptsActivated[sensor->interruptNum] = 0;
    }
}


