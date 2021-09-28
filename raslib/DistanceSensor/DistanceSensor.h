/**
 * @file DistanceSensor.h
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
#pragma once 

/** General imports. */
#include <stdint.h>

/** Device specific imports. */
#include <lib/ADC/ADC.h>
#include <lib/Timer/Timer.h>


/**
 * @brief DistanceSensorConfig_t is a user defined struct that specifies a
 *        distance sensor configuration.
 */
typedef struct DistanceSensorConfig {
    /**
     * The ADC pin tied to the sensor in the GP2Y0A60SZ analog distance sensor.
     * 
     * Default for the entry is AIN0 (PE3).
     */
    enum ADCPin pin;

    /** ------------- Optional Fields. ------------- */

    /**
     * @brief The sample frequency of the distance sensor, in hz. When defined
     * to be [1, 100] hz, timer 5 with priority 5 is set to execute
     * DistanceSensorGetInt at that frequency. It fills up the values array.
     * If defined outside of this range, the timer is not enabled.
     * 
     * Default is 0, which is OFF.
     * 
     * Note that calling DistanceSensorGetInt or DistanceSensorGetBool
     * manually may have its results in the DistanceSensor_t struct values
     * member overwritten immediately.
     */
    uint8_t repeatFrequency;

    /**
     * @brief Boolean to determine whether the interrupt call for the line
     * sensor uses DistanceSensorGetBool or DistanceSensorGetInt.
     * 
     * Default false.
     */
    bool isThresholded;

    /**
     * @brief If isThresholded is enabled, then the threshold value (fixed to
     * (0, 4095]) is provided to the ensuing DistanceSensorGetBool interrupt
     * call.
     * 
     * Default is 0.
     */
    uint16_t threshold;

    /**
     * @brief The ADC module associated with the ADC Pin. There are two ADC
     * modules in the TM4C123GH6PM and both can operate simultaneously.
     *
     * Default ADC_MODULE_0.
     */
    enum ADCModule module;

    /**
     * @brief The ADC sequencer associated with the ADC Pin. There are four sequencers
     *        PER ADC module. They do not work simultaneously relative to other
     *        sequencers in the module. Multiple enabled sequencers are given
     *        priority in reverse order (e.g. SS0 goes after SS3).
     *
     * Default ADC_SS_0.
     */
    enum ADCSequencer sequencer;

    /**
     * @brief The timer ID used for this line sensor. B side timers and wide
     *        timers are currently unusable.
     * 
     * Default is TIMER_0A.
     */
    TimerID_t timer;
} DistanceSensorConfig_t;

/**
 * @brief The pregenerated struct used for containing the relevant info used for
 *        manage the DistanceSensor instance.
 */
typedef struct DistanceSensor {
    /** @brief  The ADC object that is used in the distance sensor. */
    ADC_t adc;

    /** 
     * @brief The uint16_t value associated with the ADC pin. These are filled
     * when called by DistanceSensorGetInt or DistanceSensorGetBool.
     */
    uint16_t value;
} DistanceSensor_t;

/**
 * @brief DistanceSensorInit initializes the GP2Y0A60SZ analog distance sensor.
 * 
 * @param config The configuration of the DistanceSensor module.
 * @return A DistanceSensor_t struct instance used for sampling.
 */
DistanceSensor_t DistanceSensorInit(DistanceSensorConfig_t config);

/** 
 * @brief DistanceSensorGetInt reads a 16-bit integer sensor values from [0,
 * 4095] sampled at the selected pin of the GP2Y0A60SZ distance sensor into the
 * value member of the DistanceSensor object.
 * 
 * @param sensor A pointer to the DistanceSensor_t struct.
 */
void DistanceSensorGetInt(DistanceSensor_t * sensor);

/** 
 * @brief DistanceSensorGetBool reads a 16-bit integer sensor value from [0,
 * 4095] sampled at the selected pin of the GP2Y0A60SZ distance sensor into the
 * value member of the DistanceSensor object. Using an input threshold, the
 * value is converted into a boolean integer in the range [0, 1].
 * 
 * @param sensor A pointer to the DistanceSensor_t struct.
 */
void DistanceSensorGetBool(DistanceSensor_t * sensor, uint16_t threshold);
