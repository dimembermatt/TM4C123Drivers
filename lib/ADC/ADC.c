/**
 * ADC.c
 * Devices: LM4F120; TM4C123
 * Description: Low level drivers for ADC initialization.
 * Authors: Matthew Yu.
 * Last Modified: 09/15/21
 * Note: The following parameters of the ADC are not configurable as 09/15/21:
 * - Sample Sequencer Priority (SSPRI)
 * - Interrupt handling
 * - Analog comparator, external GPIO, timer, PWM, always source triggers.
 */

/** General Imports. */
#include <stdbool.h>

/** Device specific imports. */
#include <inc/RegDefs.h>
#include <lib/ADC/ADC.h>

void HardFault_Handler(void);    // Defined in startup.s

static pin_t ADCPinMapping[12] = {
    PIN_E3,
    PIN_E2,
    PIN_E1,
    PIN_E0,
    PIN_D3,
    PIN_D2,
    PIN_D1,
    PIN_D0,
    PIN_E5,
    PIN_E4,
    PIN_B4,
    PIN_B5
};

ADC_t ADCInit(ADCConfig_t adcConfig) {
    /* 1. Enable the relevant pin GPIO. */
    GPIOConfig_t pin = {
        ADCPinMapping[adcConfig.pin],
        NONE,
        false,
        true,
        0,
        true
    };
    GPIOInit(pin);

    /* 2. Activate the clock for RCGCADC. */
    GET_REG(SYSCTL_BASE + SYSCTL_RCGCADC_OFFSET) |=
        1 << adcConfig.module;

    /* 3. Stall until clock is ready. */
    while ((GET_REG(SYSCTL_BASE + SYSCTL_PRADC_OFFSET) &
        (1 << adcConfig.module)) == 0) {};

    uint32_t moduleBase = !adcConfig.module * ADC0_BASE + adcConfig.module * ADC1_BASE;
	
    /**
     * Note: things not touched for now: 
     * - ADC_IM (interrupts)
     * - ADC_TSSEL (PWM trigger only)
     * - ADC_SPC (Sample phase control)
     * - ADC_PSSI (Sample sequence initiate - startSample func?)
     * - ADC_DCISC (Digital comparator interrupt status and clear. Not used.)
     */
			
	/* 4. Set sample rate to 125ksps. */
	GET_REG(moduleBase + ADC_PC) &= ~0xF;
	GET_REG(moduleBase + ADC_PC) |= 0x1;
		
	/* 5. Set sequencer priority to ss3=0 ... ss0=3. */
	GET_REG(moduleBase + ADC_SSPRI) = 0x0123;
			
    /* 6. Disable sample sequencer. */
    GET_REG(moduleBase + ADC_ACTSS) &= ~(1 << adcConfig.sequencer);

    /* 7. Configure conversion trigger option. */
    GET_REG(moduleBase + ADC_EMUX) &= ~(0xF << (4 * adcConfig.sequencer));
	GET_REG(moduleBase + ADC_EMUX) |= (/* TODO: adcConfig.trigger*/ 0 << (4 * adcConfig.sequencer));

    /* 8. Configure sample averaging control. */
    GET_REG(moduleBase + ADC_SAC) = adcConfig.oversampling;

    /* 9. Configure dithering. */
    GET_REG(moduleBase + ADC_CTL) = adcConfig.isDithered << 6;
	
	/* 10. Phase control. */
	GET_REG(moduleBase + ADC_SPC) = adcConfig.phase;

    /* . TODO: Interrupt control. */

    uint32_t sequencerOffset = ADC_SS0 + 0x020 * adcConfig.sequencer;

    /* 8. Select sequencer sample input channel. */
    GET_REG(moduleBase + sequencerOffset + ADC_SSMUX) &= ~(0xF << (4 * adcConfig.position));
    GET_REG(moduleBase + sequencerOffset + ADC_SSMUX) |= (adcConfig.pin << (4 * adcConfig.position));

    /* 9. Set sample sequence control variables. */
    /* TODO: note that only IEx (interrupt enable), ENDx (sample end of sequence), are asserted. */
    GET_REG(moduleBase + sequencerOffset + ADC_SSCTL) &= 
        ~(0xF << (adcConfig.position << 2));
    GET_REG(moduleBase + sequencerOffset + ADC_SSCTL) |=
		(0b0110 * !adcConfig.isNotEndSample) << (adcConfig.position << 2);

    /**
     * Note: these registers are not currently touched for now:
     * - ADC_SSOP sample sequence operation
     * - ADC_SSDC sample sequence digital comparator select
     */

    /* 8. Reenable ADCACTSS. */ 
    GET_REG(moduleBase + ADC_ACTSS) |= 1 << adcConfig.sequencer;

    ADC_t adc = {
        adcConfig.pin,
        adcConfig.module,
        adcConfig.sequencer
    };

    return adc;
}

bool ADCIsOverflow(ADC_t adc) {
    // ADC_OSTAT
    // ADC_SSFSTAT
    HardFault_Handler();
    return false;
}

bool ADCIsUnderflow(ADC_t adc) {
    // ADC_USTAT
    // ADC_SSFSTAT
    HardFault_Handler();
    return false;
}

uint32_t ADCSample(ADC_t adc) {
    /** 1. Initiate sampling in the sample sequencers. */
    uint32_t moduleBase = !adc.module * ADC0_BASE + adc.module * ADC1_BASE;
    GET_REG(moduleBase + ADC_PSSI) |=  1 << adc.sequencer;

    /** 2. Wait for flag that conversion is done. */
    while (!(GET_REG(moduleBase + ADC_RIS) & (1 << adc.sequencer))) {}

    /** 3. Read result from the FIFO. */
    uint32_t sequencerOffset = ADC_SS0 + 0x020 * adc.sequencer;
    uint32_t result = GET_REG(moduleBase + sequencerOffset +  ADC_SSFIFO);

    /** 4. Clear flag. ADC_ISC clear interrupt */
    GET_REG(moduleBase + ADC_ISC) |= 1 << adc.sequencer;

    return result;
}
