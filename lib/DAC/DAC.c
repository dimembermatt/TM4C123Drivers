/**
 * @file DAC.c
 * @author Matthew Yu (matthewjkyu@gmail.com)
 * @brief Digital to Analog Converter peripheral driver.
 * @version 0.1
 * @date 2021-09-24
 * @copyright Copyright (c) 2021
 * @note
 * Unsupported Features. This driver does not support WTimers, multiple clock
 * modes, nor count up vs count down. B-side timers are currently broken.
 */

/** General Imports. */
#include <stdbool.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>

/** Device specific imports. */
#include <inc/regDefs.h>
#include <lib/DAC/DAC.h>


/** @brief pinMap is a mapping of bit level registers to each GPIO pin. */
static struct DACMap {
    uint32_t address;
} dacMap[PORT_COUNT] = {
    0x40004000, // PA
    0x40005000, // PB
    0x40006000, // PC
    0x40007000, // PD
    0x40024000, // PE
    0x40025000, // PF
};

DAC_t DACInit(DACConfig_t config) {
    /** For each specified pin. */
    uint8_t i;
    for (i = 0; i < config.numPins; ++i) {
        assert(config.pins[i] != NULL);

        /* Initialize pin. */
        GPIOConfig_t pinConfig = {
            .pin=config.pins[i],
            .pull=GPIO_PULL_DOWN,
            .isOutput=true,
            .alternateFunction=0,
            .isAnalog=false,
            .drive=GPIO_DRIVE_2MA,
            .enableSlew=false
        };
        GPIOInit(pinConfig);
    }

    DAC_t dac = {
        .pins=config.pins,
        .numPins=config.numPins
    };

    return dac;
}

void DACOut(DAC_t dac, uint8_t data) {
    uint8_t i;
    for (i = 0; i < dac.numPins; ++i) {
		uint32_t addr = 
			dacMap[(uint8_t)(dac.pins[i] / PINS_PER_PORT)].address + 
			(0x4 << (dac.pins[i] % PINS_PER_PORT));
		uint32_t value = ((data >> i) & 0x1) << (dac.pins[i] % PINS_PER_PORT);
        GET_REG(addr) = value;
    }
}
