/**
 * File name: DistanceSensor.c
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

/** Device specific imports. */
#include <raslib/DistanceSensor/DistanceSensor.h>
#include <lib/Timers/Timers.h>


DistanceSensor_t * intSensor;
void DistanceSensorReadInterrupt(void) {
    DistanceSensorGetInt(&intSensor);
}

// TODO: make generic where distance sensors can be instantiated w/o collision.
DistanceSensor_t DistanceSensorInit(DistanceSensorConfig_t config) {
    DistanceSensor_t sensor = {
        .value=0
    };

    /* Set up the appropriate ADC. */
    ADCConfig_t adcConfig = {
        .pin=config.pin,
        .module=ADC_MODULE_0,
        .sequencer=ADC_SS_1,
        .position=ADC_SEQPOS_0,
        .isNotEndSample=false,
    };
    ADC_t adc = ADCInit(adcConfig);
    sensor.adc = adc;
    
    /* Set up a recurring timer on TIMER_3 with priority 5. */
    if (1 <= config.repeatFrequency && config.repeatFrequency <= 100) {
        intSensor = &sensor;

        /* Set up timer for interrupt. */
        TimerConfig_t timerConfig = {
            .timerID=TIMER_3A,
            .period=freqToPeriod(config.repeatFrequency, MAX_FREQ),
            .isPeriodic=true,
            .priority=5,
            .handlerTask=DistanceSensorReadInterrupt
        };
        TimerInit(timerConfig);
    }
    return sensor;
}