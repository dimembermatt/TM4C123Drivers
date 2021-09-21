/**
 * LineSensor.c
 * Devices: TM4C123
 * Description: High level driver for QTR Reflectance Sensor
 * Authors: Dario Jimenez, Matthew Yu
 * Last Modified: 09/21/2021
 * 
 * QTR reflectance Sensor information:
 * This sensor detects the reflectance of an object.
 * For instance: white reflects a high amount of light, hence the sensor will
 * detect a white object as high reflectance. In the other hand, black reflects
 * low amount of light, hence sensor will detect a black/dark object as low
 * reflectance. The reflectance captured by the sensor is converted into a
 * voltage level from 0 - 3.3V, which can be captured by the TM4C ADC as a
 * 12-bit value (0 - 4095).
 * 
 * Available TM4C123 pins for QTR sensor: 
 * - PE5, PE4, PE3, PE2, PE1, PE0
 * - PD3, PD2, PD1, PD0
 * - PB5, PB4
 * 
 * Power Specifications:
 * Connect VCC to 3.3V or 5V (VDD)
 * 100 mA input
 * It is STRONGLY RECOMMENDED to use a 5V, 5A regulator at the power input.
 * 
 * Hardware:
 * Connect pins on sensor to analog pins
 * The optimal distance for the sensors is 0.125" (3 mm)
 * Maximum recommended sensing distance: 0.25" (6mm)
 * 
 * Summary of Driver:
 * This driver initializes a specified number of sensors on the QTR as analog inputs
 * and returns the raw data from the sensor as an array of 16-bit values.
 */

/** General imports. */
#include <stdint.h>

/** Device specific imports. */
#include <raslib/LineSensor/LineSensor.h>
#include <lib/Timers/Timers.h>


static bool isThresholded = false;
static uint16_t threshold = 0;
LineSensor_t * intSensor;
void LineSensorReadInterrupt(void) {
    if (isThresholded) {
        /* Call LineSensorGetBoolArray. */
        LineSensorGetBoolArray(intSensor, threshold);
    } else {
        /* Call LineSensorGetIntArray. */
        LineSensorGetIntArray(intSensor);
    }
}

LineSensor_t LineSensorInit(LineSensorConfig_t config) {
    LineSensor_t sensor = {
        .values={0},
        .numPins=config.numPins
    };

    /* For each pin. */
    for (uint8_t i = 0; i < config.numPins; ++i) {
        /* Set up the appropriate ADC. */
        ADCConfig_t adcConfig = {
            .pin=config.pins[i],
            .position=(enum ADCSequencePosition)i,
            .isNotEndSample=i<(config.numPins-1),
        };
        ADC_t adc = ADCInit(adcConfig);
        sensor.adcs[i] = adc;
    }
    
    /* Set up a recurring timer on TIMER_3 with priority 5. */
    if (1 <= config.repeatFrequency && config.repeatFrequency <= 100) {
        /* Resize threshold if necessary and set isReadBool global. */
        if (config.isThresholded) {
            isThresholded = config.isThresholded;

            if (4095 < config.threshold) {
                config.threshold = 4095;
            }
            threshold = config.threshold;
        }
        intSensor = &sensor;

        /* Set up timer for interrupt. */
        TimerConfig_t timerConfig = {
            .timerID=TIMER_3A,
            .period=freqToPeriod(config.repeatFrequency, MAX_FREQ),
            .isPeriodic=true,
            .priority=5,
            .handlerTask=LineSensorReadInterrupt
        };
        TimerInit(timerConfig);
    }
    return sensor;
}

void LineSensorGetIntArray(LineSensor_t * sensor) {
    ADCSampleSequencer(sensor->adcs[0].module, sensor->adcs[0].sequencer, sensor->values);
}

void LineSensorGetBoolArray(LineSensor_t * sensor, uint16_t threshold) {
    ADCSampleSequencer(sensor->adcs[0].module, sensor->adcs[0].sequencer, sensor->values);
    for (uint8_t i = 0; i < sensor->numPins; ++i) {
        if (sensor->values[i] >= threshold) {
            sensor->values[i] = 1;
        } else {
            sensor->values[i] = 0;
        }
    }
}
