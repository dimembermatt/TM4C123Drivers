#include <lib/ADC/ADC.h>
#include <stdint.h>
#include <lib/DistanceSensor/DistanceSensor.h>
#include <lib/Timers/Timers.h>

void DistanceSensor_Init(DistanceSensor_t* sensor, AnalogPort_t pin){
    sensor->pin = pin;
}

void DistanceSensor_Read(DistanceSensor_t* sensor){
    ReadAnalogPort(sensor->pin, &(sensor->data));
}

