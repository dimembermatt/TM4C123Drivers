/**
 * @file PIDControllerExample.c
 * @author Matthew Yu (matthewjkyu@gmail.com)
 * @brief PID controller example showing how to execute a rudimentary PID controller and
 *        automate the tuning of PID parameters.
 * @version 0.1
 * @date 2021-09-28
 * @copyright Copyright (c) 2021
 */
#define __MAIN__ 0

/** General imports. */
#include <stdlib.h>

/** Device specific imports. */
#include <lib/PLL/PLL.h>
#include <lib/Timer/Timer.h>
#include <raslib/PIDController/PIDController.h>


void EnableInterrupts(void);    // Defined in startup.s
void DisableInterrupts(void);   // Defined in startup.s
void WaitForInterrupt(void);    // Defined in startup.s


#define MAX_ITERATIONS 50
static double history[MAX_ITERATIONS] = { 0.0 }; // For observation purposes.
static double target = 755.15;
static double sensorValue = 0.0;

/**
 * @brief plantFunction is a fake plant that simply updates the sensorValue with
 *        the input. In a more complicated system, the input is transformed into
 *        an actuation that affects the actual output (e.g. PWM -> motor speed).
 * @param input The passthrough input value.
 */
static void plantFunction(double input) {
    sensorValue = input;
}

/**
 * @brief sensorFunction is a fake sensor that return the truth - the current
 *        sensor value. In an actual system, it would return a measurement of
 *        some system output (e.g. tachometer, motor RPM).
 * @return double Sensor value.
 */
static double sensorFunction(void) {
    return sensorValue;
}

#if __MAIN__ == 0
int main(void) {
    PLLInit(BUS_80_MHZ);
    DisableInterrupts();

    DelayInit();

    PIDConfig_t pidConfig = PIDControllerInit(1000.0, 0.0, 0.0, 0.5, 0.1);

	EnableInterrupts();
    uint8_t iteration = 0;
    while (1) {
        if (iteration < MAX_ITERATIONS) {
            history[iteration] = sensorFunction();
            ++iteration;
        } else {
			while (1) {}
		}

        /* This function does three things at once. 
           1. It captures the current sensor value
           2. Iterates the PID controller to generate a new setpoint
           3. Sets the setpoint to the actuator, or the plant function. */
        plantFunction(PIDControllerStep(pidConfig, target, sensorFunction()));

        DelayMillisec(100);
    }
}

#elif __MAIN__ == 1
int main(void) {
    PLLInit(BUS_80_MHZ);
    DisableInterrupts();

    DelayInit();

    PIDConfig_t pidConfig = PIDControllerInit(1000.0, 0.0, 0.0, 0.0, 0.0);

	EnableInterrupts();
	
	/* Try with SPEED and ACCURACY modes and see what results you get. Try
	   modifying the plantFunction and making the new sensorValue a 
	   derivative result of the input. */
	pidConfig = PIDControllerTune(pidConfig, SPEED, plantFunction, sensorFunction, target, 0, 50);
    while (1) {}
}
#endif
