/**
 * @file RGB.h
 * @author Matthew Yu (matthewjkyu@gmail.com)
 * @brief Flashes the onboard TM4C RGB (PF1-3) many colors, using a state machine.
 * @version 0.1
 * @date 2021-09-27
 * @copyright Copyright (c) 2021
 */

/**
 * @brief RGBInit initializes PF2, PF3, PF4 as LEDs and TIMER_5A to update the
 *        LED states at 15 Hz. Priority 7 (least important).
 */
void RGBInit(void);

/** @brief RGBStart restarts LED state updates on TIMER_5A. */
void RGBStart(void);

/** @brief RGBStop halts LED state updates on TIMER_5A. */
void RGBStop(void);
