/**
 * File name: DistanceSensor.h
 * Devices: TM4C123
 * Description: Low-level driver for GP2Y0A60SZ analog distance sensor
 * Authors: Dario Jimenez, Matthew Yu
 * Last Modified: 09/21/21
 * 
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


typedef struct DistanceSensorConfig {
    /**
     * The ADC pin tied to the sensor in the GP2Y0A60SZ analog distance sensor.
     * 
     * Default for the entry is AIN0 (PE3).
     */
    enum ADCPin pin;

    /** ------------- Optional Fields. ------------- */

    /**
     * The sample frequency of the line sensor, in hz. When defined to be 
     * [1, 100] hz, timer 5 with priority 5 is set to execute 
     * DistanceSensorGetInt16 at that frequency. It fills up the values array.
     * If defined outside of this range, the timer is not enabled.
     * 
     * Default is 0, which is OFF.
     * 
     * Note that calling DistanceSensorGetInt16 may have its results in the
     * DistanceSensor struct values member overwritten immediately.
     */
    uint32_t repeatFrequency;
} DistanceSensorConfig_t;

typedef struct DistanceSensor {
    /**  The ADC object that is used in the distance sensor. */
    ADC_t adc;

    /** 
     * The uint16_t value associated with the ADC pin. These are filled when
     * called by DistanceSensorGetInt.
     */
    uint32_t value;
} DistanceSensor_t;

/**
 * DistanceSensorInit initializes the GP2Y0A60SZ analog distance sensor.
 * 
 * @param config The configuration of the DistanceSensor module.
 * @return A DistanceSensor_t struct instance used for sampling.
 */
DistanceSensor_t DistanceSensorInit(DistanceSensorConfig_t config);

/** 
 * DistanceSensorGetInt reads a 16-bit integer sensor values from [0, 4095] sampled
 * at the selected pin of the GP2Y0A60SZ distance sensor into the value member
 * of the DistanceSensor object.
 * 
 * @param sensor A pointer to the DistanceSensor_t struct.
 */
void DistanceSensorGetInt(DistanceSensor_t * sensor);