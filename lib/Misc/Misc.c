/**
 * File name: Misc.h
 * Devices: LM4F120; TM4C123
 * Description: Misc functions for general use.
 * Authors: Jonathan Valvano. Revised by Matthew Yu.
 * Last Modified: 03/04/21
 */

/** Device specific imports. */
#include <lib/Misc/Misc.h>


/**
 * delayMillisec delays the process by 1 ms. Tuned to the 80 MHz TM4C clock.
 * @param n The number of ms to delay the process.
 */
void delayMillisec(uint32_t n) {
    uint32_t volatile time;
    while(n) {
        time = 72724*2/91;  // 1msec, tuned at 80 MHz
        while(time) {
            time--;
        }
        n--;
    }
}
