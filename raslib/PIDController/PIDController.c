/**
 * PIDController.c
 * Devices: LM4F120; TM4C123
 * Description: PID controller driver.
 * Authors: Matthew Yu.
 * Last Modified: 04/15/21
 */

/** Device Specific imports. */
#include <lib/PIDController/PIDController.h>


/**
 * PIDControllerInit initializes a PIDConfig_t struct for later use.
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
) {
    PIDConfig_t output = {
        max,
        min,
        p,
        i,
        d
    };
    return output;
}

/**
 * PIDControllerStep runs the system input and error into the controller
 * and attempts to correct the system.
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
double PIDControllerStep(PIDConfig_t config, double desiredOutput, double actualOutput) {
    static double output = 0;
    static double error = 0;
    static double prevErr = 0;
    static double compInt = 0;
    static double compDer = 0;

    /* Calculate components. */
    error = desiredOutput - actualOutput;
    compInt += error;
    compDer = error - prevErr;
    prevErr = error;

    /* Calculate new output. */
    output = (config.p * error) + (config.i * compInt) + (config.d * compDer);

    /* Constrain the output to prevent hardware failure down the road. */
    if (output > config.max) output = config.max;
    else if (output < config.min) output = config.min;
    return output;
}

/**
 * PIDControllerTune Attempts to characterize the PID controller constants for a
 * closed loop system. Experimental.
 *
 * @param config         Initial parameters for the controller.
 * @param plantFunction  Pointer to a function to execute with the output of the
 *                       controller.
 * @param sensorFunction Pointer to a function to execute to get the output result
 *                       and thus the error. May include its own filter.
 * @param msCycleDelay   The amount of time to wait for the system to
 *                       propagate for the sensorFunction to work nominally.
 * @param numInterCycles The amount of iteration cycles the PID controller
 *                       should evaluate.
 * @param numCycles      The amount of iterations the PID controller should execute.
 * @return Optimized PID configuration.
 * @note The approximate run time is numCycles * numInterCycles * msCycleDelay.
 * @note Including a filter in the sensorFunction is highly recommended to
 *       reduce sensor noise.
 */
PIDConfig_t PIDControllerTune(
    PIDConfig_t config,
    plantFunctionPtr_t plantFunction,
    sensorFunctionPtr_t sensorFunction,
    uint32_t msCycleDelay,
    uint32_t maxCycles
) {
    PIDConfig_t endConfig = {
        config.max,
        config.min,
        0.0,
        0.0,
        0.0
    };
    return endConfig;
}
