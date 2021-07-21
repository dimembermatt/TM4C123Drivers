#ifndef DistanceSensor_H
#define DistanceSensor_H

#include <stdint.h>
#include <lib/ADC/ADC.h>
#include <lib/GPIO/GPIO.h>
#include <lib/Timers/Timers.h>


typedef struct DistanceSensor{
    AnalogPort_t OUT;
    GPIOConfig_t EN; //enable pin
    int8_t isEnabled;
    uint8_t isInterrupt;
    uint8_t interruptNum;
    TimerConfig_t* interruptConfig;
    uint16_t data;
}DistanceSensor_t;

typedef enum{
    ENABLED,
    DISABLED
}SensorStatus_t;


//TODO: Maybe add a function that shows if sensor is enabled or disabled

void DistanceSensor_Init(DistanceSensor_t* sensor, AnalogPort_t OUT_pin, GPIOConfig_t EN_pin);

void DistanceSensor_Enable(DistanceSensor_t* sensor);

void DistanceSensor_Disable(DistanceSensor_t* sensor);

SensorStatus_t DistanceSensor_ShowStatus(DistanceSensor_t sensor);

void DistanceSensor_Read(DistanceSensor_t* sensor);

void DistanceSensor_SetInterrupt(DistanceSensor_t* sensor, uint32_t frequency, TimerID_t timer, uint8_t priority);

void DistanceSensor_DisableInterrupt(DistanceSensor_t* sensor);

#endif
