#include <lib/ADC/ADC.h>
#include <stdint.h>
#include <lib/DistanceSensor/DistanceSensor.h>
#include <lib/Timers/Timers.h>
#include <lib/GPIO/GPIO.h>

//TODO: ADD a "initialized" field to DistanceSensor_t to check if not initialize (Maybe)

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
    if(sensor.isEnabled = 1) return ENABLED;

    else return DISABLED;
}

void DistanceSensor_Read(DistanceSensor_t* sensor){
    ReadAnalogPort(sensor->OUT, &(sensor->data));
}

