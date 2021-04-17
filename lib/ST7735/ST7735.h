/**
 * File name: ST7735.h
 * Devices: LM4F120; TM4C123
 * Description: Low level drivers for the ST7735 160x128 LCD.
 * Authors: Jonathan Valvano. Revised by Matthew Yu.
 * Last Modified: 04/17/21
 */
#pragma once

/** General imports. */
#include <stdint.h>


/** Flags for ST7735_InitR(). */
enum initRFlags{
  none,
  INITR_GREENTAB,
  INITR_REDTAB,
  INITR_BLACKTAB
};

#define ST7735_TFTWIDTH  128
#define ST7735_TFTHEIGHT 160

/** Color definitions. */
#define ST7735_BLACK   0x0000
#define ST7735_BLUE    0xF800
#define ST7735_RED     0x001F
#define ST7735_GREEN   0x07E0
#define ST7735_CYAN    0xFFE0
#define ST7735_MAGENTA 0xF81F
#define ST7735_YELLOW  0x07FF
#define ST7735_WHITE   0xFFFF


/** Initialization routines for the ST7735. */


/**
 * ST7735InitB Initializes ST7735B screens.
 */
void ST7735InitB(void);

/**
 * ST7735InitR Initializes ST7735R screens (green or red tab stickers).
 * @param option One of the enumerated options depending on the tab sticker.
 */
void ST7735InitR(enum initRFlags option);

/**
 * ST7735Init is the standard screen initialization routine.
 */
void ST7735Init(void);


/** Screen Management Routines. */


/** ST7735Clear clears the display. */
void ST7735Clear(void);

/** 
 * ST7735Off Puts the display into a minimum power consumption mode. 
 * NOTE: Experimental.
 */
void ST7735Off(void);

/**
 * ST7735On turns on the display after being turned off by ST7735Off.
 * NOTE: Experimental.
 */
void ST7735On(void);

/**
 * ST7735SetRotation sets the rotation of the image.
 * @param m New rotation value (0 to 3).
 */
void ST7735SetRotation(uint8_t m);

/**
 * ST7735InvertDisplay inverts the display colors.
 * @param i 0 to disable inversion, non-zero to enable inversion.
 */
void ST7735InvertDisplay(int i);

/**
 * ST7735SetTextCursor Moves the cursor to the desired X and Y position. The next
 * character will be printed here. Typically used for graphing.
 * @param x New cursor X position. (0 to 20)
 * @param y New cursor Y position. (0 to 15)
 */
void ST7735SetTextCursor(uint32_t x, uint32_t y);

/**
 * ST7735SetTextColor sets the color for all future text output.
 * @param color 16-bit RGB text color.
 */
void ST7735SetTextColor(uint16_t color);

/**
 * ST7735FillScreen fills the screen with a given color.
 * Requires 40, 971 bytes of transmission.
 * @param color 16-bit color to display for the pixel. Can be produced by
 *              ST7735_Color565().
 */
void ST7735FillScreen(uint16_t color);


/** Basic Drawing Routines. */


/**
 * ST7735Color565 passes a 8-bit (each) R,G,B and gets back 16-bit packed color.
 * @param r Red value
 * @param g Green value
 * @param b Blue value
 * @return 16-bit packed color (RGB) value.
 */
uint16_t ST7735Color565(uint8_t r, uint8_t g, uint8_t b);

/**
 * ST7735SwapRedBlue swaps the red and blue values of a given 16-bit packed
 * color. Green is unchanged.
 * @param x 16-bit packed color value.
 * @return 16-bit packed color (RGB) value with R and B swapped.
 */
uint16_t ST7735SwapRedBlue(uint16_t x);

/**
 * ST7735DrawPixel draws a pixel at the given coordinates with the given color.
 * @param x     X coordinate of the pixel starting from the left side with
 *              domain [0, 128).
 * @param y     Y coordinate of the pixel starting from the top edge with range
 *              [0, 160).
 * @param color 16-bit color to display for the pixel. Can be produced by
 *              ST7735_Color565().
 */
void ST7735DrawPixel(int16_t x, int16_t y, uint16_t color);

/**
 * ST7735DrawFastVLine draws a vertical line at the given coordinates with the
 * given height and color.
 * Requires (11 + 2*h) bytes of transmission (assuming image fully on screen).
 * @param x X coordinate of the line.
 * @param y Y coordinate of the start of the line, from the top edge.
 * @param h Vertical height of the line going downwards from (X, Y).
 * @param color 16-bit color to display for the pixel. Can be produced by
 *              ST7735_Color565().
 */
void ST7735DrawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color);

/**
 * ST7735DrawFastHLine draws a vertical line at the given coordinates with the
 * given height and color.
 * Requires (11 + 2*h) bytes of transmission (assuming image fully on screen).
 * @param x X coordinate of the start of the line, from the left edge.
 * @param y Y coordinate of the line.
 * @param h Vertical height of the line going downwards from (X, Y).
 * @param color 16-bit color to display for the pixel. Can be produced by
 *              ST7735_Color565().
 */
void ST7735DrawFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color);

/**
 * ST7735FillRect Draws a filled rectangle given coordinates, width, height, and
 * color. 
 * @param x X coordinate of the top left corner of the rectangle.
 * @param y Y coordinate of the top left corner of the rectangle.
 * @param w Width of the rectangle.
 * @param h Height of the rectangle.
 * @param color 16-bit color to display for the pixel. Can be produced by
 *              ST7735_Color565().
 */
void ST7735FillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);

/**
 * ST7735DrawSmallCircle Draws a 6 pixel diameter circle at the given
 * coordinates with the given color.
 * @param x X coordinate of the top left corner of the circle.
 * @param y Y coordinate of the top left corner of the circle.
 * @param color 16-bit color to display for the pixel. Can be produced by
 *              ST7735_Color565().
 */
void ST7735DrawSmallCircle(int16_t x, int16_t y, uint16_t color);

/**
 * ST7735DrawCircle Draws a 10 pixel diameter circle at the given
 * coordinates with the given color.
 * Requires (11*10+68*2)=178 bytes of transmission (assuming image on screen).
 * @param x X coordinate of the top left corner of the circle.
 * @param y Y coordinate of the top left corner of the circle.
 * @param color 16-bit color to display for the pixel. Can be produced by
 *              ST7735_Color565().
 */
void ST7735DrawCircle(int16_t x, int16_t y, uint16_t color);

/**
 * ST7735DrawLine draws a line onto the display.
 * @param x1 Starting X coordinate.
 * @param y1 Starting Y coordinate.
 * @param x2 Ending X coordinate.
 * @param y2 Ending Y coordinate.
 * @param color 16-bit color to display for the pixel. Can be produced by
 *              ST7735_Color565().
 */
void ST7735DrawLine(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color);


/** Advanced Drawing Routines. */


/**
 * ST7735DrawBitmap displays a 16-bit color BMP image.
 * 
 * A bitmap file that is created by a PC image processing program has a header
 * and may be padded with dummy columns so the data have four byte alignment.
 * This function assumes that all of that has been stripped out, and the array
 * image[] has one 16-bit halfword for each pixel to be displayed on the screen
 * (encoded in reverse order, which is standard for bitmap files). An array can
 * be created in this format from a 24-bit-per-pixel .bmp file using the
 * associated converter program.
 * 
 * (x,y) is the screen location of the lower left corner of BMP image.
 * Requires (11 + 2*w*h) bytes of transmission (assuming image fully on screen).
 * @param x X coordinate of the bottom left corner of the rectangle.
 * @param y Y coordinate of the bottom left corner of the rectangle.
 * @param w Width of the image in pixels.
 * @param h Height of the image in pixels.
 * @param image Reference to the 16-bit color BMP image.
 * 
 * NOTE: Must be less than or equal to 128 pixels wide by 160 pixels high.
 */
void ST7735DrawBitmap(int16_t x, int16_t y, int16_t w, int16_t h, const uint16_t *image);


/** Text and Number Manipulation. */


/**
 * ST7735DrawChar draws a single character on the screen.
 * Requires (11 + size*size*6*8) bytes of transmission (assuming image fully on
 * screen). Drawn irrespective of where the cursor is.
 * @param x Column that the character is displayed on. From the left edge.
 * @param y Row that the character is displayed on. From the top edge.
 * @param c Character to be printed.
 * @param textColor Character RGB color.
 * @param bgColor Background RGB color.
 * @param size Number of pixels per character pixel. Scales the character across
 *             multiple rows and columns.
 */
void ST7735DrawChar(int16_t x, int16_t y, char c, int16_t textColor, int16_t bgColor, uint8_t size);

/**
 * ST7735DrawString draws a null terminated string on the screen.
 * Requires (11 + size*size*6*8) bytes of transmission for each character.
 * Drawn irrespective of where the cursor is.
 * @param x Column that the character is displayed on. From the left edge.
 * @param y Row that the character is displayed on. From the top edge.
 * @param pt Pointer to a null terminated string to be printed.
 * @param textColor Character RGB color.
 * @param bgColor Background RGB color.
 * @return uint32_t The number of characters printed.
 * NOTE: Text size is set to 1.
 */
uint32_t ST7735DrawString(uint16_t x, uint16_t y, char *pt, int16_t textColor, int16_t bgColor);

/**
 * ST7735DrawCharAtCursor draws a single character on the screen.
 * Position determined by ST7735SetTextCursor. Font, size, and color are fixed.
 * @param ch Character to be printed.
 */
void ST7735DrawCharAtCursor(char ch);

/**
 * ST7735DrawStringAtCursor draws a null terminated string on the screen.
 * Position determined by ST7735SetTextCursor. Font, size, and color are fixed.
 * @param ptr Pointer to a null terminated string to be printed.
 */
void ST7735DrawStringAtCursor(char *ptr);

/**
 * ST7735OutUDec outputs a 32-bit number as an unsigned decimal.
 * Position determined by ST7735SetCursor.
 * Color set by ST7735SetTextColor.
 * Variable format of 1-10 digits with no space before or after.
 * @param n 32 bit number to display.
 */
void ST7735OutUDec(uint32_t n);

/**
 * ST7735OutUDec outputs a 32-bit number as an unsigned 4-digit decimal.
 * Position determined by ST7735SetCursor.
 * Color set by ST7735SetTextColor.
 * @param n 32 bit number to display.
 */
void ST7735OutUDec4(uint32_t n);

/**
 * ST7735OutUDec outputs a 32-bit number as an unsigned 5-digit decimal.
 * Position determined by ST7735SetCursor.
 * Color set by ST7735SetTextColor.
 * @param n 32 bit number to display.
 */
void ST7735OutUDec5(uint32_t n);

/**
 * ST7735FixedDecOut2 Converts a fixed point number into a set of ASCII values
 * to the display. Range is -99.00 to +99.99.
 * @param n Signed integer as a fixed point value.
 * 
 * Parameter LCD display output
 *  12345    " **.**"
 *   2345    " 23.45"  
 *  -8100    "-81.00"
 *   -102    " -1.02" 
 *     31    "  0.31" 
 * -12345    "-**.**"
 */
void ST7735FixedDecOut2(int32_t n);

/**
 * ST7735UBinOut6 Converts a binary fixed point number into a set of ASCII
 * values to the display with a resolution of 1/64. Output range is 0 to 999.99.
 * @param n Unsigned 32-bit integer part of binary fixed-point number. Valid
 * inputs are 0 to 63999.
 * 
 * Parameter LCD display output
 *     0    "  0.00"
 *     1    "  0.01"
 *    16    "  0.25"
 *    25    "  0.39"
 *   125    "  1.95"
 *   128    "  2.00"
 *  1250    " 19.53"
 *  7500    "117.19"
 * 63999    "999.99"
 * 64000    "***.**"
 */
void ST7735UBinOut6(uint32_t n); 


/** Plotting and Graphing. */


/**
 * ST7735PlotInit sets up a scatter plot.
 * @param title ASCII string to label the plot. Null terminated.
 * @param minX Smallest X data value allowed, resolution=0.001.
 * @param maxX Largest X data value allowed, resolution=0.001.
 * @param minY Smallest Y data value allowed, resolution=0.001.
 * @param maxY Largest Y data value allowed, resolution=0.001.
 * Assumes minX < maxX; minY < maxY.
 */
void ST7735PlotInit(char *title, int32_t minX, int32_t maxX, int32_t minY, int32_t maxY);

/**
 * ST7735PlotInitWithoutReset sets up a scatter plot without clearing the screen.
 * @param title ASCII string to label the plot. Null terminated.
 * @param minX Smallest X data value allowed, resolution=0.001.
 * @param maxX Largest X data value allowed, resolution=0.001.
 * @param minY Smallest Y data value allowed, resolution=0.001.
 * @param maxY Largest Y data value allowed, resolution=0.001.
 * Assumes minX < maxX; minY < maxY.
 */
void ST7735PlotInitWithoutReset(char *title, int32_t minX, int32_t maxX, int32_t minY, int32_t maxY);

/**
 * ST7735Plot plots an array of (X, Y) points.
 * @param num Number of data points in each array.
 * @param bufX Array of 32-bit fixed point data, resolution=0.001.
 * @param bufY Array of 32-bit fixed point data, resolution=0.001.
 * Assumes ST7735PlotInit has been previously called and neglects points outside
 * of bounds.
 */
void ST7735Plot(uint32_t num, int32_t bufX[], int32_t bufY[]);

/**
 * ST7735DrawLineGraph draws a line graph onto the display.
 * @param num Number of data points in each array.
 * @param bufX Array of 32-bit fixed point data, resolution=0.001.
 * @param bufY Array of 32-bit fixed point data, resolution=0.001.
 * @param color RGB line and point color.
 */
void ST7735DrawLineGraph(uint32_t num, int32_t bufX[], int32_t bufY[], uint16_t color);
