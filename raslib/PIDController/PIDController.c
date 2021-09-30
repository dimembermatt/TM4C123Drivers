/**
 * @file PIDController.h
 * @author Matthew Yu (matthewjkyu@gmail.com)
 * @brief PID Controller driver.
 * @version 0.1
 * @date 2021-09-28
 * @copyright Copyright (c) 2021
 */

/** General imports. */
#include <math.h>

/** Device Specific imports. */
#include <raslib/PIDController/PIDController.h>
#include <lib/Timer/Timer.h>


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

PIDConfig_t PIDControllerTune(
    PIDConfig_t config,
    enum TuneMode mode,
    void (*plantFunction)(double input),
    double (*sensorFunction)(void),
    double desiredOutput,
    uint32_t msCycleDelay,
    uint32_t numCycles
) {

    double pMultiplier = 0.1;
    double iMultiplier = 0.1;
    double dMultiplier = 0.1;
	uint32_t convergenceID = 0;

    const uint8_t DIM_LENGTH = 20;
            /* Each iteration lasts numIterCycles. During this period, we use the
            PID controller to actuate our plant and try to minimize the error. */
    if (mode == ACCURACY) {
        double convergenceError = 1.0;
        /* For each iteration, we want to attempt a new set of PID terms. */
        uint32_t p;
        for (p = 0; p < DIM_LENGTH; ++p) {
            config.p = p * pMultiplier;
            uint32_t i;
            for (i = 0; i < DIM_LENGTH; ++i) {
                config.i = i * iMultiplier;
                uint32_t d;
                for (d = 0; d < DIM_LENGTH; ++d) {
                    config.d = d * dMultiplier;

                    /* Reset plant. */
					double actualOutput = 0.0;
                    plantFunction(0.0);

                    /* Delay the cycle by msCycleDelay for the new input to propagate. */
                    DelayMillisec(msCycleDelay);

					double outputHist[5] = {0.0};
                    uint32_t iter;
                    for (iter = 0; iter < numCycles; ++iter) {
						outputHist[iter%5] = sensorFunction();
						
						/* This function does three things at once. 
						   1. It captures the current sensor value
						   2. Iterates the PID controller to generate a new setpoint
						   3. Sets the setpoint to the actuator, or the plant function. */
						plantFunction(PIDControllerStep(config, desiredOutput, sensorFunction()));

                        /* Delay the cycle by msCycleDelay for the new input to propagate. */
                        DelayMillisec(msCycleDelay);
                    }
					outputHist[numCycles%5] = sensorFunction();
					double outputHistTailAvg = 
						(outputHist[0] + 
						 outputHist[1] + 
						 outputHist[2] + 
						 outputHist[3] + 
						 outputHist[4]) / 5;

                    /* Evaluate the end error. */
                    double error = fabs(desiredOutput - outputHistTailAvg) / desiredOutput;
                    if (error < convergenceError) {
                        convergenceID = p << 16 | i << 8 | d;
                        convergenceError = error;
                    }
                }
            }
        }
    } else if (mode == SPEED) {
        uint32_t convergenceCycles = numCycles;
        /* For each iteration, we want to attempt a new set of PID terms. */
        uint32_t p;
        for (p = 0; p < DIM_LENGTH; ++p) {
            config.p = p * pMultiplier;
            uint32_t i;
            for (i = 0; i < DIM_LENGTH; ++i) {
                config.i = i * iMultiplier;
                uint32_t d;
                for (d = 0; d < DIM_LENGTH; ++d) {
                    config.d = d * dMultiplier;

                    /* Reset plant. */
                    plantFunction(0.0);

                    /* Delay the cycle by msCycleDelay for the new input to propagate. */
                    DelayMillisec(msCycleDelay);

					double outputHist[5] = {0.0};
                    uint32_t iter;
                    for (iter = 0; iter < numCycles; ++iter) {
						outputHist[iter%5] = sensorFunction();
						double outputHistTailAvg = 
							(outputHist[0] + 
							 outputHist[1] + 
							 outputHist[2] + 
							 outputHist[3] + 
							 outputHist[4]) / 5;

					
						/* This function does three things at once. 
						   1. It captures the current sensor value
						   2. Iterates the PID controller to generate a new setpoint
						   3. Sets the setpoint to the actuator, or the plant function. */
						plantFunction(PIDControllerStep(config, desiredOutput, sensorFunction()));

                        /* Delay the cycle by msCycleDelay for the new input to propagate. */
                        DelayMillisec(msCycleDelay);
	
                        /* Evaluate convergence speed. */
                        if (fabs(desiredOutput - outputHistTailAvg) / desiredOutput < 0.05 && iter < convergenceCycles) {
                            convergenceID = p << 16 | i << 8 | d;
                            convergenceCycles = iter;
                            break;
                        }
                    }
                }
            }
        }
    }

    /* Set config. */
    config.p = ((convergenceID & 0xFF0000) >> 16) * pMultiplier;
    config.i = ((convergenceID & 0x00FF00) >> 8) * iMultiplier;
    config.d = ((convergenceID & 0x0000FF) >> 0) * dMultiplier;

    return config;
}
