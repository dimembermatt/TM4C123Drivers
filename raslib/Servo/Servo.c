/**
 * @file Servo.c
 * @author Matthew Yu (matthewjkyu@gmail.com)
 * @brief High level driver for running a SM-S4303R Springrc servo motor.
 * @version 0.1
 * @date 2021-09-29
 * @copyright Copyright (c) 2021
 */

/** General imports. */
#include <assert.h>

/** Device Specific imports. */
#include <raslib/Servo/Servo.h>


// PWM_t ServoInit(PWMPin_t pin) {
//     PWMConfig_t config = {
//         .source=PWM_SOURCE_DEFAULT,
//         .sourceInfo.pin=pin,
//         .period=freqToPeriod(333, MAX_FREQ), // 3 ms
//         .dutyCycle=50
//     };
//     return PWMInit(config);
// }

PWM_t ServoInit(ServoConfig_t config) {
    PWMConfig_t pmwConfig = {
        .source=PWM_SOURCE_TIMER,
        .sourceInfo.timerSelect.pin=config.pin,
        .sourceInfo.timerSelect.timerID=config.timerID,
        .period=240000, // 3 ms
        .dutyCycle=50
    };
    return PWMInit(pmwConfig);
}

void ServoSetSpeed(PWM_t servo, int8_t speed) {
    assert(-100 <= speed && speed <= 100);

    /* 120 000 cycles at 80 MHZ is 1.5 ms pulse width, where the servo moves
       neither clockwise or counter clockwise. 40 000 cycles is 0.5 ms.

       The documentation is not great, but it's somewhat implied that the
       range of pulse width is 1 ms to 2 ms, the former going CCW and the latter
       going CW. 

       We should scale the input speed from [-100, 100] to [1ms, 2ms].
       In this case, the minimum duty cycle to reach 1ms is 33, and the maximum
       duty cycle to reach 2ms is 67, so our mapping is from [-100, 100] -> [33,
       67].  */

    uint8_t dutyCycle = (uint8_t)(50.0 + 0.165 * speed); 
    PWMUpdateConfig(servo, 240000, dutyCycle);
}

void ServoStart(PWM_t servo) {
    PWMStart(servo);
}

void ServoStop(PWM_t servo) {
    PWMStop(servo);
}
