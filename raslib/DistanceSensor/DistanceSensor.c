/**
 * @file DistanceSensor.c
 * @author Dario Jimenez, Matthew Yu (matthewjkyu@gmail.com)
 * @brief High level driver for the GP2Y0A60SZ analog distance sensor.
 * @version 0.1
 * @date 2021-09-27
 * @copyright Copyright (c) 2021
 * @note
 * GP2Y0A60SZLF Analog Distance Sensor information:
 * This sensor measures the time of flight of an infrared transceiver pair. The
 * time of flight corresponds to a distance value.
 * - Range of measuring distance: 10 cm to 80 cm 
 * - update period: 38.3 ± 9.6 ms
 * 
 * Available TM4C123 pins for distance sensor: 
 * - PE5, PE4, PE3, PE2, PE1, PE0
 * - PD3, PD2, PD1, PD0
 * - PB5, PB4
 * 
 * Power Specifications:
 * - Supply voltage: 4.5 - 5.5 V
 * - Supply current: ~30 mA
 * - Use your 5V regulator! Do not draw from TM4C.
 * 
 * Pinout:
 * - VCC pin: 5V input line
 * - GND pin: GND input line
 * - OUT pin: Analog output
 * 
 * Summary of driver:
 * - this driver allows you to initialize distance sensors, read an unsigned
 *   16-bit value from the sensors, and set up periodic interrupts where data
 *   is read from a sensor at a desired frequency.
 */

/** General imports. */
#include <assert.h>

/** Device specific imports. */
#include <raslib/DistanceSensor/DistanceSensor.h>


#define NUM_ADC_MODULES 2
#define NUM_ADC_SEQUENCERS 4

/** @brief Settings used to manage all possible DistanceSensor configurations. */
static struct DistanceSensorSettings {
    /** @brief Sensor reference to feed the values into */
    DistanceSensor_t * sensor;

    /** @brief Whether DistanceSensorGetBool should be called. */
    bool isThresholded;

    /** @brief The thresholding value inserted into DistanceSensorGetBool. */
    uint16_t threshold;

} sensorSettings[NUM_ADC_MODULES * NUM_ADC_SEQUENCERS];

/**
 * @brief DistanceSensorReadInterrupt is the generic handler passed to the ADC timer
 *        based interrupt function. It samples the ADC.
 *
 * @param args A pointer to a list of arguments. In this function, arg[0] should
 *             be a pointer to an entry in sensorSettings.
 */
void DistanceSensorReadInterrupt(uint32_t * args) {
    struct DistanceSensorSettings * setting = ((struct DistanceSensorSettings *)args);
    if (setting->isThresholded) {
        /* Call DistanceSensorGetBoolArray. */
        DistanceSensorGetBool(setting->sensor, setting->threshold);
    } else {
        /* Call DistanceSensorGetIntArray. */
        DistanceSensorGetInt(setting->sensor);
    }
}

DistanceSensor_t DistanceSensorInit(DistanceSensorConfig_t config) {
    /* Initialization asserts. */
    assert(config.repeatFrequency <= 100);
    assert(config.threshold <= 4095);

    DistanceSensor_t sensor = {
        .value=0
    };

    /* Set up the appropriate ADC. */
    ADCConfig_t adcConfig = {
        .pin=config.pin,
        .module=config.module,
        .sequencer=config.sequencer,
        .position=ADC_SEQPOS_0,
        .isNotEndSample=false,
    };
    ADC_t adc = ADCInit(adcConfig);
    sensor.adc = adc;
    
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
            .timerTask=DistanceSensorReadInterrupt,
            .isPeriodic=true,
            .priority=5,
            .timerArgs=(uint32_t *)&sensorSettings[idx]
        };
        TimerInit(timerConfig);
    }
    return sensor;
}

void DistanceSensorGetInt(DistanceSensor_t * sensor) {
    ADCSampleSequencer(sensor->adc.module, sensor->adc.sequencer, &(sensor->value));
}

void DistanceSensorGetBool(DistanceSensor_t * sensor, uint16_t threshold) {
    ADCSampleSequencer(sensor->adc.module, sensor->adc.sequencer, &(sensor->value));
    if (sensor->value >= threshold) {
        sensor->value = 1;
    } else {
        sensor->value = 0;
    }
}
