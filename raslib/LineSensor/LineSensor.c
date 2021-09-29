/**
 * @file LineSensor.h
 * @author Dario Jimenez, Matthew Yu (matthewjkyu@gmail.com)
 * @brief High level driver for the QTR Reflectance Sensor
 * @version 0.1
 * @date 2021-09-27
 * @copyright Copyright (c) 2021
 * @note
 * QTR reflectance Sensor information:
 * This sensor detects the reflectance of an object.
 * For instance: white reflects a high amount of light, hence the sensor will
 * detect a white object as high reflectance. In the other hand, black reflects
 * low amount of light, hence sensor will detect a black/dark object as low
 * reflectance. The reflectance captured by the sensor is converted into a
 * voltage level from 0 - 3.3V, which can be captured by the TM4C ADC as a
 * 12-bit value (0 - 4095).
 * - The optimal distance for the sensors is 3 mm
 * - Maximum recommended sensing distance is 6 mm
 * 
 * Available TM4C123 pins for QTR sensor: 
 * - PE5, PE4, PE3, PE2, PE1, PE0
 * - PD3, PD2, PD1, PD0
 * - PB5, PB4
 * 
 * Power Specifications:
 * - Supply voltage: 3.3 - 5.0 V
 * - Supply current: 100 mA
 * - Use your 5V regulator! Do not draw from TM4C.
 * 
 * Pinout:
 * - VCC pin: 5V input line or 3.3V input line. If latter, short the 3.3V bypass.
 * - GND pin: GND input line
 * - 1-8 pins: ADC inputs, in order
 * 
 * Summary of driver:
 * - this driver allows you to initialize line sensors, read an unsigned
 *   16-bit value from the sensors, and set up periodic interrupts where data
 *   is read from a sensor at a desired frequency.
 */

/** General imports. */
#include <assert.h>

/** Device specific imports. */
#include <raslib/LineSensor/LineSensor.h>


#define NUM_ADC_MODULES 2
#define NUM_ADC_SEQUENCERS 4

/** @brief Settings used to manage all possible LineSensor configurations. */
static struct LineSensorSettings {
    /** @brief Sensor reference to feed the values into */
    LineSensor_t * sensor;

    /** @brief Whether LineSensorGetBoolArray should be called. */
    bool isThresholded;

    /** @brief The thresholding value inserted into LineSensorGetBoolArray. */
    uint16_t threshold;

} sensorSettings[NUM_ADC_MODULES * NUM_ADC_SEQUENCERS];

/**
 * @brief LineSensorReadInterrupt is the generic handler passed to the ADC timer
 *        based interrupt function. It samples the ADC.
 *
 * @param args A pointer to a list of arguments. In this function, arg[0] should
 *             be a pointer to an entry in sensorSettings.
 */
void LineSensorReadInterrupt(uint32_t * args) {
    struct LineSensorSettings * setting = ((struct DistanceSensorSettings *)args);
    if (setting->isThresholded) {
        /* Call LineSensorGetBoolArray. */
        LineSensorGetBoolArray(setting->sensor, setting->threshold);
    } else {
        /* Call LineSensorGetIntArray. */
        LineSensorGetIntArray(setting->sensor);
    }
}

LineSensor_t LineSensorInit(LineSensorConfig_t config) {
    /* Initialization asserts. */
    assert(config.numPins <= MAX_PINS_QTR_8);
    assert(config.repeatFrequency <= 100);
    assert(config.threshold <= 4095);

    LineSensor_t sensor = {
        .values={0},
        .numPins=config.numPins
    };

    /* For each pin. */
    uint8_t i;
    for (i = 0; i < config.numPins; ++i) {
        /* Set up the appropriate ADC. */
        ADCConfig_t adcConfig = {
            .pin=config.pins[i],
            .module=config.module,
            .sequencer=config.sequencer,
            .position=(enum ADCSequencePosition)i,
            .isNotEndSample=i<(config.numPins-1),
        };
        ADC_t adc = ADCInit(adcConfig);
        sensor.adcs[i] = adc;
    }
    
    /* Set up a recurring timer on TIMER_3 with priority 5. */
    if (1 <= config.repeatFrequency && config.repeatFrequency <= 100) {
        uint8_t idx = config.module * 4 + config.sequencer;

        sensorSettings[idx].isThresholded = config.isThresholded;
        sensorSettings[idx].threshold = config.threshold;
        sensorSettings[idx].sensor = &sensor;

        /* Set up timer for interrupt. */
        TimerConfig_t timerConfig = {
            .timerID=config.timer,
            .period=freqToPeriod(config.repeatFrequency, MAX_FREQ),
            .timerTask=LineSensorReadInterrupt,
            .isPeriodic=true,
            .priority=5,
            .timerArgs=(uint32_t *)&sensorSettings[idx]
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
    uint8_t i;
    for (i = 0; i < sensor->numPins; ++i) {
        if (sensor->values[i] >= threshold) {
            sensor->values[i] = 1;
        } else {
            sensor->values[i] = 0;
        }
    }
}
