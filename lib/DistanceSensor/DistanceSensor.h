#ifndef DistanceSensor_H
#define DistanceSensor_H

#include <stdint.h>
#include <lib/ADC/ADC.h>

typedef struct DistanceSensor{
    AnalogPort_t pin;
    uint16_t data;
}DistanceSensor_t;


void DistanceSensor_Init(DistanceSensor_t* sensor, AnalogPort_t pin);

void DistanceSensor_Read(DistanceSensor_t* sensor);

void DistanceSensor_SetInterrupt(uint16_t period, uint16_t* data);

#endif

