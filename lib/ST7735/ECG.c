/**
 * File name: ECG.c
 * Devices: LM4F120; TM4C123
 * Description: Abstracted driver calls for the ST7735 for displaying
 * electrocardiograms, or more general voltage versus time graphs.
 * Authors: Jonathan Valvano. Revised by Matthew Yu.
 * Last Modified: 04/17/21
 */

/** Device specific imports. */
#include <lib/ST7735/ECG.h>


/**
 * ECGClear clears the existing plot and wipes the screen.
 * The range of the plot are reset as well.
 * @param min Minimum range of the ECG.
 * @param max Maximum range of the ECG.
 */
void ECGClear(int32_t min, int32_t max) {
    ST7735Clear();
    if (max > min) {
        yMax = max;
        yMin = min;
    } else {
        yMax = min;
        yMin = max;
    }
    xPos = 0;
}

/**
 * ECGPlotPoint plots a point at the vertical height y.
 * @param y Y coordinate of the point plotted.
 */
void ECGPlotPoint(int32_t y) {
    if (y < yMin) y = yMin;
    if (y > yMax) y = yMax;

    int32_t j = 32 + (127*(yMax - y))/(yMax - yMin);
    if (j < 32) j = 32;
    if (j > 159) j = 159;

    ST7735DrawPixel(xPos,      j,      ST7735_BLUE);
    ST7735DrawPixel(xPos + 1,  j,      ST7735_BLUE);
    ST7735DrawPixel(xPos,      j + 1,  ST7735_BLUE);
    ST7735DrawPixel(xPos + 1,  j + 1,  ST7735_BLUE);
}

/**
 * ECGPlotLine plots a line to the new point given the most recent point.
 * @param y Y coordinate of new point.
 */
void ECGPlotLine(int32_t y) {
    static int32_t lastj = 0;
    int32_t i, j;

    if (y < yMin) y = yMin;
    if (y > yMax) y = yMax;
    
    j = 32 + (127*(yMax - y))/(yMax - yMin);
    if (j < 32) j = 32;
    if (j > 159) j = 159;
    if (lastj < 32) lastj = j;
    if (lastj > 159) lastj = j;

    if (lastj < j) {
        for (i = lastj + 1; i <= j ; i++) {
            ST7735DrawPixel(xPos,      i,  ST7735_BLUE);
            ST7735DrawPixel(xPos + 1,  i,  ST7735_BLUE);
        }
    } else if(lastj > j) {
        for (i = j; i < lastj ; i++) {
            ST7735DrawPixel(xPos,      i,  ST7735_BLUE);
            ST7735DrawPixel(xPos + 1,  i,  ST7735_BLUE);
        }
    } else {
        ST7735DrawPixel(xPos,      j,  ST7735_BLUE);
        ST7735DrawPixel(xPos + 1,  j,  ST7735_BLUE);
    }
    lastj = j;
}

/**
 * ECGPlotPoint plots two points at the vertical height y1 and y2.
 * @param y1 Y coordinate of the first point plotted.
 * @param y2 Y coordinate of the second point plotted.
 */
void ECGPlotTwoPoints(int32_t y1,int32_t y2) { 
    if (y1 < yMin) y1 = yMin;
    if (y1 > yMax) y1 = yMax;

    int32_t j = 32 + (127*(yMax - y1))/(yMax - yMin);
    if (j < 32) j = 32;
    if (j > 159) j = 159;
    ST7735DrawPixel(xPos, j, ST7735_BLUE);
    
    if (y2 < yMin) y2 = yMin;
    if (y2 > yMax) y2 = yMax;

    j = 32 + (127*(yMax - y2))/(yMax - yMin);
    if (j < 32) j = 32;
    if (j > 159) j = 159;
    ST7735DrawPixel(xPos, j, ST7735_BLACK);
}

/**
 * ECGPlotBar plots a bar at the y coordinate.
 * @param y Y coordinate to plot bar.
 */
void ECGPlotBar(int32_t y) {
    if (y < yMin) y = yMin;
    if (y > yMax) y = yMax;

    int32_t j = 32 + (127*(yMax - y))/(yMax - yMin);
    ST7735DrawFastVLine(xPos, j, 159 - j, ST7735_BLACK);
}

/**
 * ECGPlotFrequency plots a bar at y, which is 0 to 0.625 scaled on a log plot
 * from min to max. Used for amplitude vs frequency.
 * @param y ADC value of the bar plotted.
 */
void ST7735_PlotdBfs(int32_t y) {
    y /= 2; // 0 to 2047
    if (y < 0) y = 0;
    if (y > 511) y = 511;
    int32_t j = dBfs[y];
    ST7735DrawFastVLine(xPos, j, 159 - j, ST7735_BLACK);
}

/**
 * ECGStepX steps the X coordinate one pixel. Wraps around the edge of the
 * screen.
 */
void ECGStepX(void) { xPos = (xPos + 1) % 127; }

/**
 * ECGStepXAndClear steps the X coordinate one pixel. Wraps around the edge of
 * the screen. Also blanks the next spot automatically.
 */
void ECGStepXAndClear(void) {
    xPos = (xPos + 1) % 127;
    ST7735DrawFastVLine(xPos, 32, 128, ST7735Color565(228, 228, 228));
}

/**
 * ECGSetX sets the next X coordinate.
 * @param newX New X coordinate to graph on.
 */
void ECGSetX(int32_t newX) {
    if(newX > 127) xPos = 127;
    else if (newX < 0) xPos = 0;
    else xPos = newX;
}
