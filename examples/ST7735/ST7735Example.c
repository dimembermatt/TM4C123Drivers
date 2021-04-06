/**
 * File name: ST7735Example.h
 * Devices: LM4F120; TM4C123
 * Description: Example program to demonstrate the capabilities of the ST7735
 * driver. Swap between mains to see usage of various ST7735 routines.
 * Authors: Jonathan Valvano. Modified by Matthew Yu.
 * Last Modified: 03/13/21
 * 
 * Note: Also tests the lib/SSI files.
 */

/** Device specific imports. */
#include <inc/PLL.h>
#include <lib/ST7735/ST7735.h>
#include <lib/BMP/BMP.h>


/** Prints hello world to the screen. */
int main2(void) {
    PLL_Init(Bus80MHz);
    ST7735Init();
    ST7735DrawString(0, 0, "Hello World!", ST7735_WHITE, ST7735_BLACK);
    while (1) {};
}

/** Draws circles, the string 'Char Test', and a bitmaps. */
int main(void) {
    PLL_Init(Bus80MHz);
    ST7735Init();

    /* Draw a small green circle on the top left of the screen. */
    ST7735DrawSmallCircle(10, 10, ST7735Color565(0, 255, 0));

    /* Draw a red circle on the top right of the screen. */
    ST7735DrawCircle(100, 20, ST7735Color565(255, 0, 0));

    /* Draw the strings 'Char Test' with size 1 then size 2. */
    ST7735DrawChar(0, 120, 'C', ST7735Color565(255, 0, 0), 0, 1);
    ST7735DrawChar(6, 120, 'h', ST7735Color565(0, 255, 0), 0, 1);
    ST7735DrawChar(12, 120, 'a', ST7735Color565(0, 0, 255), 0, 1);
    ST7735DrawChar(18, 120, 'r', ST7735Color565(255, 255, 0), 0, 1);
    ST7735DrawChar(24, 120, ' ', ST7735Color565(255, 255, 0), 0, 1);
    ST7735DrawChar(30, 120, 'T', ST7735Color565(0, 255, 255), 0, 1);
    ST7735DrawChar(36, 120, 'e', ST7735Color565(255, 0, 255), 0, 1);
    ST7735DrawChar(42, 120, 's', ST7735Color565(255, 255, 255), 0, 1);
    ST7735DrawChar(122, 120, 't', ST7735Color565(255, 255, 255), 0, 1);

    ST7735DrawChar(0, 140, 'C', ST7735Color565(122, 0, 0), 0, 2);
    ST7735DrawChar(12, 140, 'h', ST7735Color565(0, 122, 0), 0, 2);
    ST7735DrawChar(24, 140, 'a', ST7735Color565(0, 0, 122), 0, 2);
    ST7735DrawChar(36, 140, 'r', ST7735Color565(122, 122, 0), 0, 2);
    ST7735DrawChar(48, 140, ' ', ST7735Color565(122, 122, 0), 0, 2);
    ST7735DrawChar(60, 140, 'T', ST7735Color565(0, 122, 122), 0, 2);
    /* The last three chars are off the screen. */
    ST7735DrawChar(116, 140, 'e', ST7735Color565(122, 0, 122), 0, 2); 
    ST7735DrawChar(128, 140, 's', ST7735Color565(122, 122, 122), 0, 2);
    ST7735DrawChar(140, 140, 't', ST7735Color565(122, 122, 122), 0, 2);

    /* Draw the Windows Bliss wallpaper as a bitmap. */
    typedef struct ScreenSaver{
        char *name;             /* Image name.              */
        const uint16_t *image;  /* Image reference.         */
        int16_t w;              /* Image width.             */
        int16_t h;              /* Image height.            */
        int16_t bgColor;        /* Image background color.  */
    } ScreenSaver;
    ScreenSaver Bliss = {"Bliss", Bliss_WinXP, 42, 53, 0};
    ST7735DrawBitmap(
        ST7735_TFTWIDTH/2 - Bliss.w/2, 
        ST7735_TFTHEIGHT/2 + Bliss.h/2, 
        Bliss.w, 
        Bliss.h, 
        Bliss.image);

    while (1) {};
}
