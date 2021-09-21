/**
 * ADC.h
 * Devices: LM4F120; TM4C123
 * Description: Low level drivers for ADC initialization.
 * Authors: Matthew Yu.
 * Last Modified: 09/15/21
 */
#pragma once

/** General imports. */
#include <stdint.h>

/** Device specific imports. */
#include <lib/GPIO/GPIO.h>

enum ADCPin {
    AIN0, // PE3
    AIN1, // PE2
    AIN2, // PE1
    AIN3, // PE0
    AIN4, // PD3
    AIN5, // PD2
    AIN6, // PD1
    AIN7, // PD0
    AIN8, // PE5
    AIN9, // PE4
    AIN10,// PB4
    AIN11 // PB5
};

enum ADCModule {
    ADC_MODULE_0,
    ADC_MODULE_1
};

enum ADCSequencer {
    ADC_SS_0,
    ADC_SS_1,
    ADC_SS_2,
    ADC_SS_3
};

enum ADCSequencePosition {
    ADC_SEQPOS_0,
    ADC_SEQPOS_1,
    ADC_SEQPOS_2,
    ADC_SEQPOS_3,
    ADC_SEQPOS_4,
    ADC_SEQPOS_5,
    ADC_SEQPOS_6,
    ADC_SEQPOS_7
};

enum ADCAveraging {
    ADC_AVG_NONE,
    ADC_AVG_1,
    ADC_AVG_2,
    ADC_AVG_4,
    ADC_AVG_8,
    ADC_AVG_16,
    ADC_AVG_32,
    ADC_AVG_64
};

enum ADCPhase {
    ADC_PHASE_0,
    ADC_PHASE_22_5,
    ADC_PHASE_45,
    ADC_PHASE_67_5,
    ADC_PHASE_90,
    ADC_PHASE_112_5,
    ADC_PHASE_135,
    ADC_PHASE_157_5,
    ADC_PHASE_180,
    ADC_PHASE_202_5,
    ADC_PHASE_225,
    ADC_PHASE_247_5,
    ADC_PHASE_270,
    ADC_PHASE_292_5,
    ADC_PHASE_315,
    ADC_PHASE_337_5
};

/** Configuration of a single DAC object. */
typedef struct ADCConfig {
    /**
     * The GPIO that can be selected as ADC pins.
     * 
     * Default is AIN0, which corresponds to PE3.
     */
    enum ADCPin pin;


    /** ------------- Optional Fields. ------------- */

    /**
     * The ADC module associated with the ADC Pin. There are two ADC modules
     * in the TM4C123GH6PM and both can work simultaneously.
     *
     * Default ADC_MODULE_0.
     */
    enum ADCModule module;


    /**
     * The ADC sequencer associated with the ADC Pin. There are four sequencers
     * PER ADC module. They do not work simultaneously relative to other sequencers
     * in the module. Multiple enabled sequencers are given priority in reverse
     * order (e.g. SS0 goes after SS3).
     *
     * Default ADC_SS_0.
     */
    enum ADCSequencer sequencer;

    /**
     * The position in the sample sequencer that the ADCPin is sampled. Sequencers
     * may have the ability to sample multiple pins or the same pins in one go -
     * the position dictates where in the order of iteration the sample is captured.
     *
     * Default ADC_SEQPOS_0.
     */
    enum ADCSequencePosition position;

    /**
     * Whether the sample to configure is the last in a series of samples in the
     * sequencer to be read.
     *
     * Default False (It IS the end sample).
     */
    bool isNotEndSample;

    /**
     * The hardware sample averaging. This allows the ADC to take multiple samples
     * at once before merging them into a single entry that goes into the internal
     * FIFO that the user can pull. Hardware sample averaging smooths out noise
     * over consecutive samples and improves the accuracy of the result (see:
     * Central Limit Theorem) at the expense of power and more processing time.
     *
     * Default ADC_AVG_NONE.
     */
    enum ADCAveraging oversampling;

    /**
     * Whether the ADC samples are dithered or not. Dithering may reduce random
     * noise in the ADC sampling, especially when combined with hardware sample
     * averaging.
     *
     * Default false.
     */
    bool isDithered;

    /**
     * ADC Sample Phase Control. This causes the samples to lag by a set phase.
     * This can be used to effectively double the ADC sample rate. See P.805
     * of the datasheet.
     * Default 0.0 degrees.
     */
    enum ADCPhase phase;


    /** ------------- Unimplemented Fields. ------------- */

    /**
     * Trigger source for the ADC sample sequencer.
     * Default source is SW.
     * Analog Comparators and PWM are by default disabled and will fault.
     */
    // enum ADCTrigger { SW, AC0=0, AC1=0, GPIO=4, Timer=5, PWM0=0, PWM1=0, PWM2=0, PWM3=0, ALWAYS=0xF } trigger;

    /**
     * Interrupt control.
     * Default NULL (no interrupt).
     */
    // void (*ADCHandler)(void);
} ADCConfig_t;

typedef struct ADC {
    /**
     * The GPIO that can be selected as ADC pins.
     * Default is AIN0, which corresponds to PE3.
     * The user may modify this to a unspecified pin value - this will cause
     * undefined behavior.
     */
    enum ADCPin pin;

    /** The ADCModule associated with the ADC Pin. */
    enum ADCModule module;

    /** The ADCSequencer associated with the ADC Pin. */
    enum ADCSequencer sequencer;

    /** The ADCSequencePosition associated with the ADC Pin. */
    enum ADCSequencePosition position;
} ADC_t;


/**
 * ADCInit initializes an ADC module given an ADCConfig_t configuration.
 * 
 * @param config The configuration of the ADC module.
 * @return An ADC_t struct instance used for sampling.
 */
ADC_t ADCInit(ADCConfig_t config);

/**
 * ADCIsEmpty checks whether the sequencer at the given ADC module has no
 * samples available.
 * 
 * @param module ADC module to check.
 * @param sequencer ADC module's sequencer to check.
 * @return If the sequencer FIFO is empty and no samples can be retrieved.
 */
bool ADCIsEmpty(enum ADCModule module, enum ADCSequencer sequencer);

/**
 * ADCIsFull checks whether the sequencer at the given ADC module cannot capture
 * any more samples.
 * 
 * @param module ADC module to check.
 * @param sequencer ADC module's sequencer to check.
 * @return If the sequencer FIFO is full and no samples can be captured.
 */
bool ADCIsFull(enum ADCModule module, enum ADCSequencer sequencer);

/**
 * ADCSampleSingle samples a single ADC pin. This may be slower if the sequencer
 * is configured to capture other samples, as this method cycles through the
 * internal sample FIFO to capture the wanted value and throw out the others.
 *
 * @param adc The ADC object containing the pin value that should be sampled.
 * @return A single uint32_t representing the pin ADC value sampled.
 */
uint32_t ADCSampleSingle(ADC_t adc);

/**
 * ADCSampleSequencer samples a single sequencer and returns all the values in the
 * FIFO accumulated, up to 8 values. This may be faster than ADCSampleSingle on a
 * per sample basis. Guarantees that the internal FIFO is empty beforehand.
 *
 * @param module The ADCModule to execute.
 * @param sequencer The ADCSequencer to sequence.
 * @param arr A reference to an array to fill with values.
 */
void ADCSampleSequencer(enum ADCModule module, enum ADCSequencer sequencer, uint32_t arr[8]);
