/**
 * File name: ECG.h
 * Devices: LM4F120; TM4C123
 * Description: Abstracted driver calls for the ST7735 for displaying
 * electrocardiograms, or more general voltage versus time graphs.
 * Authors: Jonathan Valvano. Revised by Matthew Yu.
 * Last Modified: 04/17/21
 */
#pragma once

/** General imports. */
#include <stdint.h>

/** Device specific imports. */
#include <lib/ST7735/ST7735.h>


// full scaled defined as 3V
// Input is 0 to 511, 0 => 159 and 511 => 32
static uint8_t const dBfs[512]={
    159, 159, 145, 137, 131, 126, 123, 119, 117, 114, 112, 110, 108, 107, 105, 104, 103, 101,
    100, 99, 98, 97, 96, 95, 94, 93, 93, 92, 91, 90, 90, 89, 88, 88, 87, 87, 86, 85, 85, 84,
    84, 83, 83, 82, 82, 81, 81, 81, 80, 80, 79, 79, 79, 78, 78, 77, 77, 77, 76, 76, 76, 75,
    75, 75, 74, 74, 74, 73, 73, 73, 72, 72, 72, 72, 71, 71, 71, 71, 70, 70, 70, 70, 69, 69,
    69, 69, 68, 68, 68, 68, 67, 67, 67, 67, 66, 66, 66, 66, 66, 65, 65, 65, 65, 65, 64, 64,
    64, 64, 64, 63, 63, 63, 63, 63, 63, 62, 62, 62, 62, 62, 62, 61, 61, 61, 61, 61, 61, 60,
    60, 60, 60, 60, 60, 59, 59, 59, 59, 59, 59, 59, 58, 58, 58, 58, 58, 58, 58, 57, 57, 57,
    57, 57, 57, 57, 56, 56, 56, 56, 56, 56, 56, 56, 55, 55, 55, 55, 55, 55, 55, 55, 54, 54,
    54, 54, 54, 54, 54, 54, 53, 53, 53, 53, 53, 53, 53, 53, 53, 52, 52, 52, 52, 52, 52, 52,
    52, 52, 52, 51, 51, 51, 51, 51, 51, 51, 51, 51, 51, 50, 50, 50, 50, 50, 50, 50, 50, 50,
    50, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48,
    48, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 46, 46, 46, 46, 46, 46, 46, 46, 46,
    46, 46, 46, 46, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 44, 44, 44, 44, 44,
    44, 44, 44, 44, 44, 44, 44, 44, 44, 43, 43, 43, 43, 43, 43, 43, 43, 43, 43, 43, 43, 43,
    43, 43, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 41, 41, 41, 41, 41,
    41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40,
    40, 40, 40, 40, 40, 40, 39, 39, 39, 39, 39, 39, 39, 39, 39, 39, 39, 39, 39, 39, 39, 39,
    39, 39, 38, 38, 38, 38, 38, 38, 38, 38, 38, 38, 38, 38, 38, 38, 38, 38, 38, 38, 38, 37,
    37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 36, 36, 36, 36,
    36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 35, 35, 35, 35, 35,
    35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 34, 34, 34, 34, 34, 34,
    34, 34, 34, 34, 34, 34, 34, 34, 34, 34, 34, 34, 34, 34, 34, 34, 34, 33, 33, 33, 33, 33,
    33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 32, 32, 32,
    32, 32, 32, 32, 32, 32, 32, 32, 32, 32
};

/** The maximum and minimum range of the ECG. */
static int32_t yMax, yMin;

/** The current X position on the screen for the most recent value. */
static int32_t xPos;

/**
 * ECGClear clears the existing plot and wipes the screen.
 * The range of the plot are reset as well.
 * @param min Minimum range of the ECG.
 * @param max Maximum range of the ECG.
 */
void ECGClear(int32_t min, int32_t max);

/**
 * ECGPlotPoint plots a point at the vertical height y.
 * @param y Y coordinate of the point plotted.
 */
void ECGPlotPoint(int32_t y);

/**
 * ECGPlotLine plots a line to the new point given the most recent point.
 * @param y Y coordinate of new point.
 */
void ECGPlotLine(int32_t y);

/**
 * ECGPlotPoint plots two points at the vertical height y1 and y2.
 * @param y1 Y coordinate of the first point plotted.
 * @param y2 Y coordinate of the second point plotted.
 */
void ECGPlotTwoPoints(int32_t y1,int32_t y2);

/**
 * ECGPlotBar plots a bar at the y coordinate.
 * @param y Y coordinate to plot bar.
 */
void ECGPlotBar(int32_t y);

/**
 * ECGPlotFrequency plots a bar at y, which is 0 to 0.625 scaled on a log plot
 * from min to max. Used for amplitude vs frequency.
 * @param y ADC value of the bar plotted.
 */
void ST7735_PlotdBfs(int32_t y);

/**
 * ECGStepX steps the X coordinate one pixel. Wraps around the edge of the
 * screen.
 */
void ECGStepX(void);

/**
 * ECGStepXAndClear steps the X coordinate one pixel. Wraps around the edge of
 * the screen. Also blanks the next spot automatically.
 */
void ECGStepXAndClear(void);

/**
 * ECGSetX sets the next X coordinate.
 * @param newX New X coordinate to graph on.
 */
void ECGSetX(int32_t newX);
