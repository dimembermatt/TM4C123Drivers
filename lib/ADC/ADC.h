/**
 * @file ADC.h
 * @author Matthew Yu (matthewjkyu@gmail.com)
 * @brief ADC peripheral driver.
 * @version 0.1
 * @date 2021-09-22
 * @copyright Copyright (c) 2021
 * @note
 * Unsupported Features. This driver does not support DMA control. This driver
 * does not support different trigger sources or interrupts. The driver does not
 * support configurable sample sequencer priorities.
 */

#pragma once

/** General imports. */
#include <stdint.h>

/** Device specific imports. */
#include <lib/GPIO/GPIO.h>


/** @brief ADCPin is an enumeration that specifies one of the available ADC pins
 *         on the TM4C. */
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
    AIN_COUNT
};

/** @brief ADCModule is an enumeration specifying of the two ADC modules. */
enum ADCModule {
    ADC_MODULE_0,
    ADC_MODULE_1
};

/** @brief ADCSequencer is an enumeration specifying of the four ADC sequencers
 *         that exist for each ADCModule. */
enum ADCSequencer {
    ADC_SS_0,
    ADC_SS_1,
    ADC_SS_2,
    ADC_SS_3
};

/** @brief ADCSequencePosition is an enumeration specifying of the eight
 *         slots in an ADC sequencer. Some sequencers may not have all eight
 *         positions available. */
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

/** @brief ADCAveraging is an enumeration specifying how many samples are taken
 *         in succession before returning an averaged result. May extend
 *         execution time when sampling. */
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

/** @brief ADCPhase is an enumeration specifying the phase offset that a given
 *         ADCModule is sampling at. */
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

/** @brief ADCConfig_t is a user defined struct that specifies an ADC pin
 *         configuration. */
typedef struct ADCConfig {
    /**
     * @brief The GPIO pin that can be selected as ADC pins.
     * 
     * Default is AIN0, which corresponds to PE3.
     */
    enum ADCPin pin;


    /** ------------- Optional Fields. ------------- */

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
     * @brief The position in the sample sequencer that the ADCPin is sampled.
     *        Sequencers may have the ability to sample multiple pins or the
     *        same pins in one go - the position dictates where in the order of
     *        iteration the sample is captured.
     *
     * Default ADC_SEQPOS_0.
     * 
     * @note Different sequencers have different number of positions.
     *       ADC_SS_0 has all 8 positions, ADC_SS_1 and ADC_SS_2 have only 4
     *       positions, and ADC_SS_3 has a single position.
     */
    enum ADCSequencePosition position;

    /**
     * @brief Whether the sample to configure is the last in a series of samples
     * in the sequencer to be read.
     *
     * Default is false (It IS the end sample).
     */
    bool isNotEndSample;

    /**
     * @brief The hardware sample averaging. This allows the ADC to take
     *        multiple samples at once before merging them into a single entry
     *        that goes into the internal FIFO that the user can pull. Hardware
     *        sample averaging smooths out noise over consecutive samples and
     *        improves the accuracy of the result (see: Central Limit Theorem)
     *        at the expense of power and more processing time. 
     *
     * Default ADC_AVG_NONE.
     */
    enum ADCAveraging oversampling;

    /**
     * @brief Whether the ADC samples are dithered or not. Dithering may reduce
     *        random noise in the ADC sampling, especially when combined with
     *        hardware sample averaging.
     *
     * Default false.
     */
    bool isDithered;

    /**
     * @brief ADC Sample Phase Control. This causes the samples to lag by a set
     *        phase. This can be used to effectively double the ADC sample rate.
     *        See P.805 of the datasheet.
     * 
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

/** @brief ADC_t is a struct containing user relevant data of an ADC. */
typedef struct ADC {
    /**
     * @brief The GPIO that can be selected as ADC pins.
     * 
     * Default is AIN0, which corresponds to PE3.
     */
    enum ADCPin pin;

    /** @brief The ADCModule associated with the ADC Pin. */
    enum ADCModule module;

    /** @brief The ADCSequencer associated with the ADC Pin. */
    enum ADCSequencer sequencer;

    /** @brief The ADCSequencePosition associated with the ADC Pin. */
    enum ADCSequencePosition position;
} ADC_t;


/**
 * @brief ADCInit initializes an ADC module given an ADCConfig_t configuration.
 * 
 * @param config The configuration of the ADC module.
 * @return An ADC_t struct instance used for sampling.
 */
ADC_t ADCInit(ADCConfig_t config);

/**
 * @brief ADCIsEmpty checks whether the sequencer at the given ADC module has no
 *        samples available.
 * 
 * @param module ADC module to check.
 * @param sequencer ADC module's sequencer to check.
 * @return If the sequencer FIFO is empty and no samples can be retrieved.
 */
bool ADCIsEmpty(enum ADCModule module, enum ADCSequencer sequencer);

/**
 * @brief ADCIsFull checks whether the sequencer at the given ADC module cannot
 *        capture any more samples.
 * 
 * @param module ADC module to check.
 * @param sequencer ADC module's sequencer to check.
 * @return If the sequencer FIFO is full and no samples can be captured.
 */
bool ADCIsFull(enum ADCModule module, enum ADCSequencer sequencer);

/**
 * @brief ADCSampleSingle samples a single ADC pin. This may be slower if the
 *        sequencer is configured to capture other samples, as this method
 *        cycles through the internal sample FIFO to capture the wanted value
 *        and throw out the others.
 *
 * @param adc The ADC object containing the pin value that should be sampled.
 * @return A single uint16_t representing the pin ADC value sampled.
 */
uint16_t ADCSampleSingle(ADC_t adc);

/**
 * @brief ADCSampleSequencer samples a single sequencer and returns all the
 *        values in the FIFO accumulated, up to 8 values. This may be faster
 *        than ADCSampleSingle on a  per sample basis. Guarantees that the
 *        internal FIFO is empty beforehand.
 *
 * @param module The ADCModule to execute.
 * @param sequencer The ADCSequencer to sequence.
 * @param arr A reference to an array to fill with values.
 */
void ADCSampleSequencer(enum ADCModule module, enum ADCSequencer sequencer, uint16_t arr[8]);
