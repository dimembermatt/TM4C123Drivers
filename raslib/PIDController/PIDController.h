/**
 * @file PIDController.h
 * @author Matthew Yu (matthewjkyu@gmail.com)
 * @brief PID Controller driver.
 * @version 0.1
 * @date 2021-09-28
 * @copyright Copyright (c) 2021
 */
#pragma once

/** General imports. */
#include <stdbool.h>
#include <stdint.h>


/*
The overall structure of the larger system is as follows:

                          environment error
                                  |
                                  v
          --------------    --------------
target -> | controller | -> |plant/system| -> output
          --------------    --------------       |
               ^                                 |
               |                                 |
               ----------------------------------+ <- sensor error

The plant represents the process; an example is the following:

                            plant
                     ------------------
controller output -> |     op amp     | -> actual output
                     |       |        |
                     |       v        |
                     |    DC motor    |
                     |       |        |
                     |       v        |
                     | rotating shaft |
                     ------------------

The PID controller belongs to the controller box, and attempts
to modulate the effects of the plant to correct the output.

                controller
          ----------------------
          | Proportional term  |
target -> |         +          |
          |    Integral term   | -> controller output
          |         +          |
 error -> |   Derivative term  |
          |---------------------

Where total error is defined as `error = output - target`.
*/

/** @brief Definition of a configuration for a PID controller. */
typedef struct PIDConfig {
    /** @brief The maximum value a control output signal can be. */
    double max;

    /** @brief The minimum value a control output signal can be. */
    double min;

    /** Tuned by user or external algorithm. */
    /* Proportional term. */
    double p;

    /* Integral term. */
    double i;

    /* Derivative term. */
    double d;
} PIDConfig_t;

/**
 * @brief PIDControllerInit initializes a PIDConfig_t struct for later use.
 *
 * @param max The maximum output value of the PIDController. Clamped.
 * @param min The minimum output value of the PIDController. Clamped.
 * @param p   The proportional term of the PIDController.
 * @param i   The integral term of the PIDController.
 * @param d   The derivative term of the PIDController.
 * @return PID controller parameters.
 */
PIDConfig_t PIDControllerInit(
    double max,
    double min,
    double p,
    double i,
    double d
);
	
enum TuneMode { ACCURACY, SPEED };

/**
 * @brief PIDControllerStep runs the system input and error into the controller
 *        and attempts to correct the system.
 *
 * @param config PID controller parameters.
 * @param desiredOutput Desired output of the system.
 * @param actualOutput  Actual output of the system.
 * @return The next input value into the plant. This is different from the input
 *         parameter.
 * @note On the first step with no output, set error to 0.
 *       A filter from Filter.h can be used to reduce sensor error
 *       and improve stability.
 *       Only one PID controller is supported for now.
 */
double PIDControllerStep(PIDConfig_t config, double desiredOutput, double actualOutput);

/**
 * @brief PIDControllerTune Attempts to characterize the PID controller
 *        constants for a closed loop system. Experimental.
 *
 * @param config         Initial parameters for the controller.
 * @param tuneMode       Selects tuning for error or speed. Latter may cause
 *                       overshooting. 
 * @param plantFunction  Pointer to a function to execute with the output of the
 *                       controller.
 * @param sensorFunction Pointer to a function to execute to get the output result
 *                       and thus the error. May include its own filter.
 * @param msCycleDelay   The amount of time to wait for the system to
 *                       propagate for the sensorFunction to work nominally.
 * @param numCycles      The amount of cycles in an iteration that the PID controller should evaluate.
 * @return Optimized PID configuration.
 * @note Including a filter in the sensorFunction is highly recommended to
 *       reduce sensor noise.
 * @note Requires DelayInit from Timer.h.
 */
PIDConfig_t PIDControllerTune(
    PIDConfig_t config,
    enum TuneMode mode,
    void (*plantFunction)(double input),
    double (*sensorFunction)(void),
    double desiredOutput,
    uint32_t msCycleDelay,
    uint32_t numCycles
);
	