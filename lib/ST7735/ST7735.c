/**
 * File name: ST7735.h
 * Devices: LM4F120; TM4C123
 * Description: Low level drivers for the ST7735 160x128 LCD.
 * Authors: Jonathan Valvano. Revised by Matthew Yu.
 * Last Modified: 04/17/21
 */

/** General imports. */
#include <stdlib.h>

/** Device specific imports. */
#include <inc/tm4c123gh6pm.h> // TODO: #2
#include <inc/RegDefs.h>
#include <lib/GPIO/GPIO.h>
#include <lib/Misc/Font.h>
#include <lib/Misc/Misc.h>
#include <lib/SSI/SSI.h>
#include <lib/ST7735/ST7735.h>


/** List of ST7735 system commands. See page 77 from STT7735_v2.1.pdf. */
#define ST7735_NOP     0x00
#define ST7735_SWRESET 0x01
#define ST7735_RDDID   0x04
#define ST7735_RDDST   0x09

#define ST7735_SLPIN   0x10
#define ST7735_SLPOUT  0x11
#define ST7735_PTLON   0x12
#define ST7735_NORON   0x13

#define ST7735_INVOFF  0x20
#define ST7735_INVON   0x21
#define ST7735_DISPOFF 0x28
#define ST7735_DISPON  0x29
#define ST7735_CASET   0x2A
#define ST7735_RASET   0x2B
#define ST7735_RAMWR   0x2C
#define ST7735_RAMRD   0x2E

#define ST7735_PTLAR   0x30
#define ST7735_MADCTL  0x36
#define ST7735_COLMOD  0x3A

#define ST7735_FRMCTR1 0xB1
#define ST7735_FRMCTR2 0xB2
#define ST7735_FRMCTR3 0xB3
#define ST7735_INVCTR  0xB4
#define ST7735_DISSET5 0xB6

#define ST7735_PWCTR1  0xC0
#define ST7735_PWCTR2  0xC1
#define ST7735_PWCTR3  0xC2
#define ST7735_PWCTR4  0xC3
#define ST7735_PWCTR5  0xC4
#define ST7735_VMCTR1  0xC5

#define ST7735_RDID1   0xDA
#define ST7735_RDID2   0xDB
#define ST7735_RDID3   0xDC
#define ST7735_RDID4   0xDD

#define ST7735_PWCTR6  0xFC

#define ST7735_GMCTRP1 0xE0
#define ST7735_GMCTRN1 0xE1

#define TFT_CS                  (*((volatile uint32_t *)0x40004020))
#define TFT_CS_LOW              0           // CS normally controlled by hardware
#define TFT_CS_HIGH             0x08
#define DC                      (*((volatile uint32_t *)0x40004100))
#define DC_COMMAND              0
#define DC_DATA                 0x40
#define RESET                   (*((volatile uint32_t *)0x40004200))
#define RESET_LOW               0
#define RESET_HIGH              0x80

#define SSI_CR0_SCR_M           0x0000FF00  // SSI Serial Clock Rate
#define SSI_CR0_SPH             0x00000080  // SSI Serial Clock Phase
#define SSI_CR0_SPO             0x00000040  // SSI Serial Clock Polarity
#define SSI_CR0_FRF_M           0x00000030  // SSI Frame Format Select
#define SSI_CR0_FRF_MOTO        0x00000000  // Freescale SPI Frame Format
#define SSI_CR0_DSS_M           0x0000000F  // SSI Data Size Select
#define SSI_CR0_DSS_8           0x00000007  // 8-bit data
#define SSI_CR1_MS              0x00000004  // SSI Master/Slave Select
#define SSI_CR1_SSE             0x00000002  // SSI Synchronous Serial Port
                                            // Enable
#define SSI_SR_BSY              0x00000010  // SSI Busy Bit
#define SSI_SR_TNF              0x00000002  // SSI Transmit FIFO Not Full
#define SSI_CPSR_CPSDVSR_M      0x000000FF  // SSI Clock Prescale Divisor
#define SSI_CC_CS_M             0x0000000F  // SSI Baud Clock Source
#define SSI_CC_CS_SYSPLL        0x00000000  // Either the system clock (if the
                                            // PLL bypass is in effect) or the
                                            // PLL output (default)
#define SYSCTL_RCGC1_SSI0       0x00000010  // SSI0 Clock Gating Control
#define SYSCTL_RCGC2_GPIOA      0x00000001  // port A Clock Gating Control
#define ST7735_TFT_WIDTH  128
#define ST7735_TFT_HEIGHT 160

/**
 * Characters based on the given fontset can be placed within 16 rows and 21
 * columns, 0 indexed.
 * Each character requires (11 + size * size * 6 * 8) bytes of transmission for
 * each character.
 */

/** Character position along the horizonal axis 0 to 20. */
uint32_t ST7735TextX=0;

/** Character position along the vertical axis 0 to 15. */
uint32_t ST7735TextY=0;

/** The default ST7735 text color. */
uint16_t ST7735TextColor = ST7735_YELLOW;

/** Some displays require a starting column and row. */
static uint8_t ST7735StartCol, ST7735StartRow;

/** ST7735 rotation. 0 to 3. */
static uint8_t ST7735Rotation;

/** ST7735 tab color stickers. */
static enum initRFlags tabColor;

/**
 * ST7735 width and height parameters. Values may change based on
 * ST7735Rotation.
 */
static int16_t _width = ST7735_TFT_WIDTH;
static int16_t _height = ST7735_TFT_HEIGHT;


/** Functions for writing commands and data to the ST7735. */


/**
 * The Data/Command pin must be valid when the eighth bit is sent. The SSI module has
 * hardware input and output FIFOs that are 8 locations deep. Based on the
 * observation that the LCD interface tends to send a few commands and then a
 * lot of data, the FIFOs are not used when writing commands, and they are used
 * when writing data. This ensures that the Data/Command pin status matches the
 * byte that is actually being transmitted.
 *
 * The write command operation waits until all data has been sent, configures
 * the Data/Command pin for commands, sends the command, and then waits for the
 * transmission to finish.
 *
 * The write data operation waits until there is room in the transmit FIFO,
 * configures the Data/Command pin for data, and then adds the data to the
 * transmit FIFO.
 *
 * NOTE: These functions will crash or stall indefinitely if the SSI0 module is
 * not initialized and enabled.
 */
void static writeCommand(uint8_t c) {
    // Wait until SSI0 not busy/transmit FIFO empty.
    while ((SSI0_SR_R & SSI_SR_BSY) == SSI_SR_BSY) {};
    TFT_CS = TFT_CS_LOW;
    DC = DC_COMMAND;
    SSI0_DR_R = c; // Data out.

    // Wait until SSI0 not busy/transmit FIFO empty.
    while ((SSI0_SR_R & SSI_SR_BSY) == SSI_SR_BSY) {};
}

void static writeData(uint8_t c) {
    // Wait until transmit FIFO not full.
    while ((SSI0_SR_R & SSI_SR_TNF) == 0) {};
    DC = DC_DATA;
    SSI0_DR_R = c; // Data out.
}

void static deselectCS(void) {
    // Wait until SSI0 not busy/transmit FIFO empty.
    while ((SSI0_SR_R & SSI_SR_BSY) == SSI_SR_BSY) {};
    TFT_CS = TFT_CS_HIGH;
}


/** ST7735 Specific Internal Commands and Data calls. */


/**
 * Rather than a bazillion writeCommand() and writeData() calls, screen
 * initialization commands and arguments are organized in these tables stored in
 * ROM. The table may look bulky, but that's mostly the formatting --
 * storage-wise this is hundreds of bytes more compact than the equivalent code.
 * Companion function follows.
 */
#define DELAY 0x80
static const uint8_t
  Bcmd[] = {                  // Initialization commands for 7735B screens
    18,                       // 18 commands in list:
    ST7735_SWRESET,   DELAY,  //  1: Software reset, no args, w/delay
      50,                     //     50 ms delay
    ST7735_SLPOUT ,   DELAY,  //  2: Out of sleep mode, no args, w/delay
      255,                    //     255 = 500 ms delay
    ST7735_COLMOD , 1+DELAY,  //  3: Set color mode, 1 arg + delay:
      0x05,                   //     16-bit color
      10,                     //     10 ms delay
    ST7735_FRMCTR1, 3+DELAY,  //  4: Frame rate control, 3 args + delay:
      0x00,                   //     fastest refresh
      0x06,                   //     6 lines front porch
      0x03,                   //     3 lines back porch
      10,                     //     10 ms delay
    ST7735_MADCTL , 1      ,  //  5: Memory access ctrl (directions), 1 arg:
      0x08,                   //     Row addr/col addr, bottom to top refresh
    ST7735_DISSET5, 2      ,  //  6: Display settings #5, 2 args, no delay:
      0x15,                   //     1 clk cycle nonoverlap, 2 cycle gate
                              //     rise, 3 cycle osc equalize
      0x02,                   //     Fix on VTL
    ST7735_INVCTR , 1      ,  //  7: Display inversion control, 1 arg:
      0x0,                    //     Line inversion
    ST7735_PWCTR1 , 2+DELAY,  //  8: Power control, 2 args + delay:
      0x02,                   //     GVDD = 4.7V
      0x70,                   //     1.0uA
      10,                     //     10 ms delay
    ST7735_PWCTR2 , 1      ,  //  9: Power control, 1 arg, no delay:
      0x05,                   //     VGH = 14.7V, VGL = -7.35V
    ST7735_PWCTR3 , 2      ,  // 10: Power control, 2 args, no delay:
      0x01,                   //     Opamp current small
      0x02,                   //     Boost frequency
    ST7735_VMCTR1 , 2+DELAY,  // 11: Power control, 2 args + delay:
      0x3C,                   //     VCOMH = 4V
      0x38,                   //     VCOML = -1.1V
      10,                     //     10 ms delay
    ST7735_PWCTR6 , 2      ,  // 12: Power control, 2 args, no delay:
      0x11, 0x15,
    ST7735_GMCTRP1,16      ,  // 13: Magical unicorn dust, 16 args, no delay:
      0x09, 0x16, 0x09, 0x20, //     (seriously though, not sure what
      0x21, 0x1B, 0x13, 0x19, //      these config values represent)
      0x17, 0x15, 0x1E, 0x2B,
      0x04, 0x05, 0x02, 0x0E,
    ST7735_GMCTRN1,16+DELAY,  // 14: Sparkles and rainbows, 16 args + delay:
      0x0B, 0x14, 0x08, 0x1E, //     (ditto)
      0x22, 0x1D, 0x18, 0x1E,
      0x1B, 0x1A, 0x24, 0x2B,
      0x06, 0x06, 0x02, 0x0F,
      10,                     //     10 ms delay
    ST7735_CASET  , 4      ,  // 15: Column addr set, 4 args, no delay:
      0x00, 0x02,             //     XSTART = 2
      0x00, 0x81,             //     XEND = 129
    ST7735_RASET  , 4      ,  // 16: Row addr set, 4 args, no delay:
      0x00, 0x02,             //     XSTART = 1
      0x00, 0x81,             //     XEND = 160
    ST7735_NORON  ,   DELAY,  // 17: Normal display on, no args, w/delay
      10,                     //     10 ms delay
    ST7735_DISPON ,   DELAY,  // 18: Main screen turn on, no args, w/delay
      255 };                  //     255 = 500 ms delay

static const uint8_t
  Rcmd1[] = {                 // Init for 7735R, part 1 (red or green tab)
    15,                       // 15 commands in list:
    ST7735_SWRESET,   DELAY,  //  1: Software reset, 0 args, w/delay
      150,                    //     150 ms delay
    ST7735_SLPOUT ,   DELAY,  //  2: Out of sleep mode, 0 args, w/delay
      255,                    //     500 ms delay
    ST7735_FRMCTR1, 3      ,  //  3: Frame rate ctrl - normal mode, 3 args:
      0x01, 0x2C, 0x2D,       //     Rate = fosc/(1x2+40) * (LINE+2C+2D)
    ST7735_FRMCTR2, 3      ,  //  4: Frame rate control - idle mode, 3 args:
      0x01, 0x2C, 0x2D,       //     Rate = fosc/(1x2+40) * (LINE+2C+2D)
    ST7735_FRMCTR3, 6      ,  //  5: Frame rate ctrl - partial mode, 6 args:
      0x01, 0x2C, 0x2D,       //     Dot inversion mode
      0x01, 0x2C, 0x2D,       //     Line inversion mode
    ST7735_INVCTR , 1      ,  //  6: Display inversion ctrl, 1 arg, no delay:
      0x07,                   //     No inversion
    ST7735_PWCTR1 , 3      ,  //  7: Power control, 3 args, no delay:
      0xA2,
      0x02,                   //     -4.6V
      0x84,                   //     AUTO mode
    ST7735_PWCTR2 , 1      ,  //  8: Power control, 1 arg, no delay:
      0xC5,                   //     VGH25 = 2.4C VGSEL = -10 VGH = 3 * AVDD
    ST7735_PWCTR3 , 2      ,  //  9: Power control, 2 args, no delay:
      0x0A,                   //     Opamp current small
      0x00,                   //     Boost frequency
    ST7735_PWCTR4 , 2      ,  // 10: Power control, 2 args, no delay:
      0x8A,                   //     BCLK/2, Opamp current small & Medium low
      0x2A,
    ST7735_PWCTR5 , 2      ,  // 11: Power control, 2 args, no delay:
      0x8A, 0xEE,
    ST7735_VMCTR1 , 1      ,  // 12: Power control, 1 arg, no delay:
      0x0E,
    ST7735_INVOFF , 0      ,  // 13: Don't invert display, no args, no delay
    ST7735_MADCTL , 1      ,  // 14: Memory access control (directions), 1 arg:
      0xC8,                   //     row addr/col addr, bottom to top refresh
    ST7735_COLMOD , 1      ,  // 15: set color mode, 1 arg, no delay:
      0x05 };                 //     16-bit color

static const uint8_t
  Rcmd2green[] = {            // Init for 7735R, part 2 (green tab only)
    2,                        //  2 commands in list:
    ST7735_CASET  , 4      ,  //  1: Column addr set, 4 args, no delay:
      0x00, 0x02,             //     XSTART = 0
      0x00, 0x7F+0x02,        //     XEND = 127
    ST7735_RASET  , 4      ,  //  2: Row addr set, 4 args, no delay:
      0x00, 0x01,             //     XSTART = 0
      0x00, 0x9F+0x01 };      //     XEND = 159

static const uint8_t
  Rcmd2red[] = {              // Init for 7735R, part 2 (red tab only)
    2,                        //  2 commands in list:
    ST7735_CASET  , 4      ,  //  1: Column addr set, 4 args, no delay:
      0x00, 0x00,             //     XSTART = 0
      0x00, 0x7F,             //     XEND = 127
    ST7735_RASET  , 4      ,  //  2: Row addr set, 4 args, no delay:
      0x00, 0x00,             //     XSTART = 0
      0x00, 0x9F };           //     XEND = 159

static const uint8_t
  Rcmd3[] = {                 // Init for 7735R, part 3 (red or green tab)
    4,                        //  4 commands in list:
    ST7735_GMCTRP1, 16      , //  1: Magical unicorn dust, 16 args, no delay:
      0x02, 0x1c, 0x07, 0x12,
      0x37, 0x32, 0x29, 0x2d,
      0x29, 0x25, 0x2B, 0x39,
      0x00, 0x01, 0x03, 0x10,
    ST7735_GMCTRN1, 16      , //  2: Sparkles and rainbows, 16 args, no delay:
      0x03, 0x1d, 0x07, 0x06,
      0x2E, 0x2C, 0x29, 0x2D,
      0x2E, 0x2E, 0x37, 0x3F,
      0x00, 0x00, 0x02, 0x10,
    ST7735_NORON  ,    DELAY, //  3: Normal display on, no args, w/delay
      10,                     //     10 ms delay
    ST7735_DISPON ,    DELAY, //  4: Main screen turn on, no args w/delay
      100 };                  //     100 ms delay

/**
 * commandList reads and issues a series of LCD commands stored in ROM byte
 * array.
 * @param addr The address to the specific set of instructions to execute.
 */
void static commandList(const uint8_t *addr) {
    uint8_t numCommands, numArgs;
    uint16_t ms;

    numCommands = *(addr++);            // Number of commands to follow
    while (numCommands--) {             // For each command...
        writeCommand(*(addr++));        //   Read, issue command
        numArgs  = *(addr++);           //   Number of args to follow
        ms       = numArgs & DELAY;     //   If hibit set, delay follows args
        numArgs &= ~DELAY;              //   Mask out delay bit
        while (numArgs--) {             //   For each argument...
            writeData(*(addr++));       //     Read, issue argument
        }

        if (ms) {
            ms = *(addr++);             // Read post-command delay time (ms)
            if (ms == 255) ms = 500;    // If 255, delay for 500 ms
            delayMillisec(ms);
        }
    }
}


/** Initialization routines for the ST7735. */


/**
 * commonInit Initialization code common to both 'B' and 'R' type displays.
 * @param cmdList
 */
void static commonInit(const uint8_t *cmdList) {
    volatile uint32_t delay;
    ST7735StartCol  = ST7735StartRow = 0; // May be overridden in init func

    /* SSI0Fss (PA3) is temporarily used as GPIO, so by default has alternative
     * functionality off. */
    GPIOConfig_t GPIOconfigs[5] = {
        {PIN_A2, PULL_DOWN, true,  true,  2, false},
        {PIN_A3, PULL_DOWN, true,  true,  2, false},
        {PIN_A5, PULL_DOWN, true,  false, 2, false},
        {PIN_A6, PULL_DOWN, true,  false, 0, false},
        {PIN_A7, PULL_DOWN, true,  false, 0, false}
    };

    /* Initialize SysTick for delay calls.*/
    delayInit();
    
    /* 1. Initialize GPIO. */
    for (uint8_t i = 0; i < 5; i++) {
        GPIOInit(GPIOconfigs[i]);
    }

    /* 2. Toggle RST low to reset; CS low so it'll listen to us. */
    TFT_CS = TFT_CS_LOW;
    RESET = RESET_HIGH;
    delayMillisec(500);
    RESET = RESET_LOW;
    delayMillisec(500);
    RESET = RESET_HIGH;
    delayMillisec(500);

    /* 3. Turn PA3 SSI0Fss alternative functionality back on again. */
    GPIOconfigs[2].isAlternative = true;
    GPIOInit(GPIOconfigs[2]);

    /* 5. Initialize SSI0. This reinitializes the gpio pins, but putting this in
     *    front and just manipulating PA5 doesn't work, probably because SSI0 is
     *    enabled.*/
    SSIConfig_t SSI0Config = {
        SSI0_PA, FREESCALE_SPI, true, true, true, true, 0x8
    };
    SSIInit(SSI0Config);

    if (cmdList) commandList(cmdList);
}

/**
 * ST7735InitB Initializes ST7735B screens.
 */
void ST7735InitB(void) {
    commonInit(Bcmd);
    ST7735SetTextCursor(0, 0);
    ST7735TextColor = ST7735_YELLOW;
    ST7735FillScreen(ST7735_BLACK);
}

/**
 * ST7735InitR Initializes ST7735R screens (green or red tab stickers).
 * @param option One of the enumerated options depending on the tab sticker.
 */
void ST7735InitR(enum initRFlags option) {
    commonInit(Rcmd1);
    if (option == INITR_GREENTAB) {
        commandList(Rcmd2green);
        ST7735StartCol = 2;
        ST7735StartRow = 1;
    } else {
        // ST7735StartCol, ST7735StartRow left at default '0' values
        commandList(Rcmd2red);
    }
    commandList(Rcmd3);

    // if black, change MADCTL color filter
    if (option == INITR_BLACKTAB) {
        writeCommand(ST7735_MADCTL);
        writeData(0xC0);
    }
    tabColor = option;
    ST7735SetTextCursor(0,0);
    ST7735TextColor = ST7735_YELLOW;
    ST7735FillScreen(ST7735_BLACK);
}

/**
 * ST7735Init is the standard screen initialization routine.
 */
void ST7735Init(void) {
    ST7735InitR(INITR_REDTAB);
    ST7735FillScreen(ST7735_BLACK);
}

/**
 * setAddrWindow Sets the region of the screen RAM to be modified. Pixel colors
 * are sent left to right, top to bottom (same as Font table is encoded;
 * different from regular bitmap).
 *
 * Requires 11 bytes of transmission.
 *
 * @param x0 x start
 * @param y0 y start
 * @param x1 x end
 * @param y1 y end
 */
void static setAddrWindow(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1) {
    // Set the column address.
    writeCommand(ST7735_CASET);
    writeData(0x00);
    writeData(x0+ST7735StartCol);
    writeData(0x00);
    writeData(x1+ST7735StartCol);

    // Set the row address.
    writeCommand(ST7735_RASET);
    writeData(0x00);
    writeData(y0+ST7735StartRow);
    writeData(0x00);
    writeData(y1+ST7735StartRow);

    // Write to RAM.
    writeCommand(ST7735_RAMWR);
}

/**
 * pushColor Send 2 bytes of data, most significant byte first, consisting of the
 * color to display for the pixel.
 * @param color Color to display for the pixel.
 */
void static pushColor(uint16_t color) {
    writeData((uint8_t)(color >> 8));
    writeData((uint8_t)color);
}


/** Screen Management Routines. */


/** ST7735Clear clears the display. */
void ST7735Clear(void) {
    ST7735FillScreen(ST7735_BLACK);
}

/**
 * ST7735Off Puts the display into a minimum power consumption mode.
 * NOTE: Experimental.
 * TODO: #3
 */
void ST7735Off(void) {
    writeCommand(ST7735_DISPOFF);
    delayMillisec(500);
    writeCommand(ST7735_SLPIN);
    delayMillisec(500);
}

/**
 * ST7735On turns on the display after being turned off by ST7735Off.
 * NOTE: Experimental.
 * TODO: #3
 */
void ST7735On(void) {
    writeCommand(ST7735_SLPOUT);
    delayMillisec(500);
    writeCommand(ST7735_DISPON);
    delayMillisec(500);
}

#define MADCTL_MY  0x80
#define MADCTL_MX  0x40
#define MADCTL_MV  0x20
#define MADCTL_ML  0x10
#define MADCTL_RGB 0x00
#define MADCTL_BGR 0x08
#define MADCTL_MH  0x04

/**
 * ST7735SetRotation sets the rotation of the image.
 * @param m New rotation value (0 to 3).
 */
void ST7735SetRotation(uint8_t m) {
    writeCommand(ST7735_MADCTL);
    ST7735Rotation = m % 4;
    switch (ST7735Rotation) {
        case 0:
            if (tabColor == INITR_BLACKTAB) writeData(MADCTL_MX | MADCTL_MY | MADCTL_RGB);
            else writeData(MADCTL_MX | MADCTL_MY | MADCTL_BGR);
            _width  = ST7735_TFT_WIDTH;
            _height = ST7735_TFT_HEIGHT;
            break;
        case 1:
            if (tabColor == INITR_BLACKTAB) writeData(MADCTL_MY | MADCTL_MV | MADCTL_RGB);
            else writeData(MADCTL_MY | MADCTL_MV | MADCTL_BGR);
            _width  = ST7735_TFT_HEIGHT;
            _height = ST7735_TFT_WIDTH;
            break;
        case 2:
            if (tabColor == INITR_BLACKTAB) writeData(MADCTL_RGB);
            else writeData(MADCTL_BGR);
            _width  = ST7735_TFT_WIDTH;
            _height = ST7735_TFT_HEIGHT;
            break;
        case 3:
            if (tabColor == INITR_BLACKTAB) writeData(MADCTL_MX | MADCTL_MV | MADCTL_RGB);
            else writeData(MADCTL_MX | MADCTL_MV | MADCTL_BGR);
            _width  = ST7735_TFT_HEIGHT;
            _height = ST7735_TFT_WIDTH;
            break;
    }
    deselectCS();
}

/**
 * ST7735InvertDisplay inverts the display colors.
 * @param i 0 to disable inversion, non-zero to enable inversion.
 */
void ST7735InvertDisplay(int i) {
    if (i) writeCommand(ST7735_INVON);
    else writeCommand(ST7735_INVOFF);
    deselectCS();
}

/**
 * ST7735SetTextCursor Moves the cursor to the desired X and Y position. The next
 * character will be printed here. Typically used for graphing.
 * @param x New cursor X position. (0 to 20)
 * @param y New cursor Y position. (0 to 15)
 */
void ST7735SetTextCursor(uint32_t x, uint32_t y) {
    if ((x > 20) || (y > 15)) return;
    ST7735TextX = x;
    ST7735TextY = y;
}

/**
 * ST7735SetTextColor sets the color for all future text output.
 * @param color 16-bit RGB text color.
 */
void ST7735SetTextColor(uint16_t color) {
    ST7735TextColor = color;
}

/**
 * ST7735FillScreen fills the screen with a given color.
 * Requires 40, 971 bytes of transmission.
 * @param color 16-bit color to display for the pixel. Can be produced by
 *              ST7735Color565().
 */
void ST7735FillScreen(uint16_t color) {
    // The screen is actually 129 by 161 pixels, but we don't use the extra.
    ST7735FillRect(0, 0, _width, _height, color);
}


/** Basic Drawing Routines. */


/**
 * ST7735Color565 passes a 8-bit (each) R,G,B and gets back 16-bit packed color.
 * @param r Red value
 * @param g Green value
 * @param b Blue value
 * @return 16-bit packed color (RGB) value with R and B swapped.
 */
uint16_t ST7735Color565(uint8_t r, uint8_t g, uint8_t b) {
  return ((b & 0xF8) << 8) | ((g & 0xFC) << 3) | (r >> 3);
}

/**
 * ST7735SwapRedBlue swaps the red and blue values of a given 16-bit packed
 * color. Green is unchanged.
 * @param x 16-bit packed color value.
 * @return 16-bit packed color (RGB) value with R and B swapped.
 */
uint16_t ST7735SwapRedBlue(uint16_t x) {
  return (x << 11) | (x & 0x07E0) | (x >> 11);
}

/**
 * ST7735DrawPixel draws a pixel at the given coordinates with the given color.
 * @param x     X coordinate of the pixel starting from the left side with
 *              domain [0, 128).
 * @param y     Y coordinate of the pixel starting from the top edge with range
 *              [0, 160).
 * @param color 16-bit color to display for the pixel. Can be produced by
 *              ST7735Color565().
 */
void ST7735DrawPixel(int16_t x, int16_t y, uint16_t color) {
    if ((x < 0) || (x >= _width) || (y < 0) || (y >= _height)) return;
    //  setAddrWindow(x,y,x+1,y+1); // TODO: #3 original code, bug???
    setAddrWindow(x,y,x,y);
    pushColor(color);
    deselectCS();
}

/**
 * ST7735DrawFastVLine draws a vertical line at the given coordinates with the
 * given height and color.
 * Requires (11 + 2*h) bytes of transmission (assuming image fully on screen).
 * @param x X coordinate of the line.
 * @param y Y coordinate of the start of the line, from the top edge.
 * @param h Vertical height of the line going downwards from (X, Y).
 * @param color 16-bit color to display for the pixel. Can be produced by
 *              ST7735Color565().
 */
void ST7735DrawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color) {
    // Rudimentary clipping.
    if ((x >= _width) || (y >= _height)) return;
    if ((y + h - 1) >= _height) h = _height - y;
    setAddrWindow(x, y, x, y + h - 1);

    while (h--) pushColor(color);
    deselectCS();
}

/**
 * ST7735DrawFastHLine draws a vertical line at the given coordinates with the
 * given height and color.
 * Requires (11 + 2*h) bytes of transmission (assuming image fully on screen).
 * @param x X coordinate of the start of the line, from the left edge.
 * @param y Y coordinate of the line.
 * @param h Vertical height of the line going downwards from (X, Y).
 * @param color 16-bit color to display for the pixel. Can be produced by
 *              ST7735Color565().
 */
void ST7735DrawFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color) {
    // Rudimentary clipping.
    if ((x >= _width) || (y >= _height)) return;
    if ((x + w - 1) >= _width)  w = _width - x;
    setAddrWindow(x, y, x + w - 1, y);

    while (w--) pushColor(color);
    deselectCS();
}

/**
 * ST7735FillRect Draws a filled rectangle given coordinates, width, height, and
 * color.
 * @param x X coordinate of the top left corner of the rectangle.
 * @param y Y coordinate of the top left corner of the rectangle.
 * @param w Width of the rectangle.
 * @param h Height of the rectangle.
 * @param color 16-bit color to display for the pixel. Can be produced by
 *              ST7735Color565().
 */
void ST7735FillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color) {
    // Rudimentary clipping.
    if ((x >= _width) || (y >= _height)) return;
    if ((x + w - 1) >= _width)  w = _width  - x;
    if ((y + h - 1) >= _height) h = _height - y;
    setAddrWindow(x, y, x + w - 1, y + h - 1);

    for (y = h; y > 0; y--) {
        for (x = w; x > 0; x--) {
            pushColor(color);
        }
    }
    deselectCS();
}

int16_t const smallCircle[6][3]={
    {2,3,    2},
  {1  ,  4,  4},
 {0   ,   5, 6},
 {0   ,   5, 6},
  {1  ,  4,  4},
    {2,3,    2}};

int16_t const circle[10][3]={
      { 4,5,         2},
    {2   ,   7,      6},
  {1     ,      8,   8},
  {1     ,      8,   8},
 {0      ,       9, 10},
 {0      ,       9, 10},
  {1     ,      8,   8},
  {1     ,      8,   8},
    {2   ,    7,     6},
     {  4,5,         2}};

/**
 * ST7735DrawSmallCircle Draws a 6 pixel diameter circle at the given
 * coordinates with the given color.
 * Requires (11*6+24*2)=114 bytes of transmission (assuming image on screen).
 * @param x X coordinate of the top left corner of the circle.
 * @param y Y coordinate of the top left corner of the circle.
 * @param color 16-bit color to display for the pixel. Can be produced by
 *              ST7735Color565().
 */
void ST7735DrawSmallCircle(int16_t x, int16_t y, uint16_t color) {
    // Rudimentary clipping.
    if ((x > _width - 5) || (y > _height - 5)) return;
    for (uint32_t i = 0; i < 6; i++) {
        setAddrWindow(x + smallCircle[i][0], y + i, x + smallCircle[i][1], y + i);
        uint32_t w = smallCircle[i][2];
        while (w--) pushColor(color);
    }
    deselectCS();
}

/**
 * ST7735DrawCircle Draws a 10 pixel diameter circle at the given
 * coordinates with the given color.
 * Requires (11*10+68*2)=178 bytes of transmission (assuming image on screen).
 * @param x X coordinate of the top left corner of the circle.
 * @param y Y coordinate of the top left corner of the circle.
 * @param color 16-bit color to display for the pixel. Can be produced by
 *              ST7735Color565().
 */
void ST7735DrawCircle(int16_t x, int16_t y, uint16_t color) {
    // Rudimentary clipping.
    if ((x > _width - 9) || (y > _height - 9)) return; // doesn't fit
    for (uint32_t i = 0; i < 10; i++) {
        setAddrWindow(x + circle[i][0], y + i, x + circle[i][1], y + i);
        uint32_t w = circle[i][2];
        while (w--) pushColor(color);
    }
    deselectCS();
}


/**
 * ST7735DrawHorizontalLine plots a line where dX > dY.
 * @param x1 X value of starting point.
 * @param y1 Y value of starting point.
 * @param x2 X value of ending point.
 * @param y2 Y value of ending point.
 * @param color RGB line color.
 */
void ST7735DrawHorizontalLine(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color) {
    int32_t d = 0;
    int32_t dx = x2 - x1;
    int32_t dy = y2 - y1;
    int32_t plotY = y1;

    d = 2*abs(dy) - dx;
    for (int32_t plotX = x1; plotX < x2; plotX++) {
        if (abs(dx) > abs(dy)) {
            ST7735DrawPixel(plotX, plotY, color);
            ST7735DrawPixel(plotX, plotY+1, color);
        } else {
            ST7735DrawPixel(plotX, plotY, color);
            ST7735DrawPixel(plotX+1, plotY, color);
        }
        if (d > 0) {
            if (dy < 0) {
                plotY--;
            } else {
                plotY++;
            }
            d -= 2*dx;
        }
        d += 2*abs(dy);
    }
}

/**
 * ST7735DrawHorizontalLine plots a line where dX < dY.
 * @param x1 X value of starting point.
 * @param y1 Y value of starting point.
 * @param x2 X value of ending point.
 * @param y2 Y value of ending point.
 * @param color RGB line color.
 */
void ST7735DrawVerticalLine(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color) {
    int32_t d = 0;
    int32_t dx = x2- x1;
    int32_t dy = y2 - y1;
    int32_t plotX = x1;

    d = 2*abs(dx) - dy;
    for (int32_t plotY = y1; plotY < y2; plotY++) {
        if (abs(dx) > abs(dy)) {
            ST7735DrawPixel(plotX, plotY, color);
            ST7735DrawPixel(plotX, plotY+1, color);
        } else {
            ST7735DrawPixel(plotX, plotY, color);
            ST7735DrawPixel(plotX+1, plotY, color);
        }
        if (d > 0) {
            if (dx < 0) {
                plotX--;
            } else {
                plotX++;
            }
            d -= 2*dy;
        }
        d += 2*abs(dx);
    }
}

/**
 * ST7735DrawLine draws a line onto the display.
 * @param x1 Starting X coordinate.
 * @param y1 Starting Y coordinate.
 * @param x2 Ending X coordinate.
 * @param y2 Ending Y coordinate.
 * @param color 16-bit color to display for the pixel. Can be produced by
 *              ST7735_Color565().
 */
void ST7735DrawLine(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color) {
    if (abs(y2 - y1) < abs(x2 - x1)) {
        if (x1 < x2) ST7735DrawHorizontalLine(x1, y1, x2, y2, color);
        else ST7735DrawHorizontalLine(x2, y2, x1, y1, color);
    } else {
        if (y1 < y2) ST7735DrawVerticalLine(x1, y1, x2, y2, color);
        else ST7735DrawVerticalLine(x2, y2, x1, y1, color);
    }
}


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
 * TODO: #3 Speed this up.
 */
void ST7735DrawBitmap(int16_t x, int16_t y, int16_t w, int16_t h, const uint16_t *image) {
    int16_t skipC = 0;                      // non-zero if columns need to be skipped due to clipping.
    int16_t originalWidth = w;              // save this value; even if not all columns fit on the screen, the image is still this width in ROM.
    int i = w * (h - 1);

    // image is totally off the screen, do nothing
    if ((x >= _width) || ((y - h + 1) >= _height) || ((x + w) <= 0) || (y < 0)) return;

    /* image is too wide for the screen, do nothing. This isn't necessarily a
     * fatal error, but it makes the following logic much more complicated, since you
     * can have an image that exceeds multiple boundaries and needs to be clipped on
     * more than one side. */
    if ((w > _width) || (h > _height)) return;

    if ((x + w - 1) >= _width) {            // image exceeds right of screen
        skipC = (x + w) - _width;           // skip cut off columns
        w = _width - x;
    }
    if ((y - h + 1) < 0) {                  // image exceeds top of screen
        i = i - (h - y - 1) * originalWidth;// skip the last cut off rows
        h = y + 1;
    }
    if (x < 0) {                            // image exceeds left of screen
        w = w + x;
        skipC = -1*x;                       // skip cut off columns
        i = i - x;                          // skip the first cut off columns
        x = 0;
    }
    if (y >= _height) {                     // image exceeds bottom of screen
        h = h - (y - _height + 1);
        y = _height - 1;
    }

    setAddrWindow(x, y-h+1, x+w-1, y);

    for(y = 0; y < h; y++) {
        for(x = 0; x < w; x++) {
            // send the top and bottom 8 bits, then go to the next pixel.
            writeData((uint8_t)(image[i] >> 8));
            writeData((uint8_t)image[i]);
            i++;
        }
        i = i + skipC;
        i = i - 2 * originalWidth;
    }

    deselectCS();
}


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
 * @param size Number of pixels per character pizel. Scales the character across
 *             multiple rows and columns.
 */
void ST7735DrawChar(int16_t x, int16_t y, char c, int16_t textColor, int16_t bgColor, uint8_t size) {
    if (((x + 6*size - 1) >= _width) ||  // Clip right
        ((y + 8*size - 1) >= _height) || // Clip bottom
        ((x + 6*size - 1) < 0)        || // Clip left
        ((y + 8*size - 1) < 0)){         // Clip top
        return;
    }
    setAddrWindow(x, y, x + 6*size - 1, y + 8*size - 1);

    int32_t col, row, i, j;
    uint8_t line = 0x01; // horizontal row of pixels of character.

    // print the rows, starting at the top.
    for (row = 0; row < 8; row++) {
        for (i = 0; i < size; i++) {
            // print the columns, starting on the left
            for (col = 0; col < 5; col++) {
                if (Font[(c*5) + col] & line) {
                    // bit is set in Font, print pixel(s) in text color
                    for (j = 0; j < size; j++) pushColor(textColor);
                } else {
                    // bit is cleared in Font, print pixel(s) in background color
                    for (j = 0; j < size; j++) pushColor(bgColor);
                }
            }
            // print blank column(s) to the right of character
            for (j = 0; j < size; j++) pushColor(bgColor);
        }
        line = line << 1;   // move up to the next row
    }
    deselectCS();
}

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
uint32_t ST7735DrawString(uint16_t x, uint16_t y, char *pt, int16_t textColor, int16_t bgColor) {
    if (y > 15) return 0;

    uint32_t count = 0;
    while (*pt) {
        ST7735DrawChar(x*6, y*10, *pt, textColor, bgColor, 1);
        pt++;
        if (++x > 20) return count;
        count++;
    }
    return count;
}

/**
 * ST7735DrawCharAtCursor draws a single character on the screen.
 * Position determined by ST7735SetTextCursor. Font, size, and color are fixed.
 * @param ch Character to be printed.
 */
void ST7735DrawCharAtCursor(char ch) {
    if ((ch == 10) || (ch == 13) || (ch == 27)) {
        ST7735TextY++;
        ST7735TextX = 0;
        if (ST7735TextY > 15) ST7735TextY = 0;
        ST7735DrawString(0, ST7735TextY, "                     ", ST7735TextColor, ST7735_BLACK);
        return;
    }
    ST7735DrawChar(ST7735TextX*6, ST7735TextY*10, ch, ST7735_YELLOW, ST7735_BLACK, 1);
    ST7735TextX++;
    if (ST7735TextX > 20) {
        ST7735TextX = 20;
        ST7735DrawChar(ST7735TextX*6, ST7735TextY*10, '*', ST7735_RED, ST7735_BLACK, 1);
    }
    return;
}

/**
 * ST7735DrawStringAtCursor draws a null terminated string on the screen.
 * Position determined by ST7735SetTextCursor. Font, size, and color are fixed.
 * @param ptr Pointer to a null terminated string to be printed.
 */
void ST7735DrawStringAtCursor(char *ptr) {
    while (*ptr) {
        ST7735DrawCharAtCursor(*ptr);
        ptr++;
    }
}

/** Buffer used for storing decimal messages for displaying on the ST7735. */
char decimalMessage[12];

/** Index for determining where in the decimalMessage buffer we are. */
uint32_t decimalMessageIdx;

/**
 * fillDecimalMessage converts a decimal number of unspecified length as an
 * ASCII string.
 * @param n Decimal number to convert.
 */
void fillDecimalMessage(uint32_t n){
    if (n >= 10) {
        fillDecimalMessage(n/10);
        n = n % 10;
    }
    decimalMessage[decimalMessageIdx] = (n + '0'); /* n is between 0 and 9 */
    if (decimalMessageIdx < 11) decimalMessageIdx++;
}

/**
 * fillDecimalMessage4 converts a four digit decimal number into an ASCII
 * string. Saturates at 9999.
 * @param n 4 digit decimal number to convert.
 */
void fillDecimalMessage4(uint32_t n){
    if (n > 9999) n = 9999;
    if (n >= 1000){
        decimalMessageIdx = 0;
    } else if (n >= 100) {
        decimalMessage[0] = ' ';
        decimalMessageIdx = 1;
    } else if (n >= 10) {
        decimalMessage[0] = ' ';
        decimalMessage[1] = ' ';
        decimalMessageIdx = 2;
    } else {
        decimalMessage[0] = ' ';
        decimalMessage[1] = ' ';
        decimalMessage[2] = ' ';
        decimalMessageIdx = 3;
    }
    fillDecimalMessage(n);
}

/**
 * fillDecimalMessage5 converts a five digit decimal number into an ASCII
 * string.
 * @param n 5 digit decimal number to convert.
 */
void fillDecimalMessage5(uint32_t n){
    if (n > 99999) n = 99999;
    if (n >= 10000) {
        decimalMessageIdx = 0;
    } else if (n >= 1000) {
        decimalMessage[0] = ' ';
        decimalMessageIdx = 1;
    } else if (n >= 100) {
        decimalMessage[0] = ' ';
        decimalMessage[1] = ' ';
        decimalMessageIdx = 2;
    } else if (n >= 10) {
        decimalMessage[0] = ' ';
        decimalMessage[1] = ' ';
        decimalMessage[2] = ' ';
        decimalMessageIdx = 3;
    } else {
        decimalMessage[0] = ' ';
        decimalMessage[1] = ' ';
        decimalMessage[2] = ' ';
        decimalMessage[3] = ' ';
        decimalMessageIdx = 4;
    }
    fillDecimalMessage(n);
}

/**
 * ST7735OutUDec outputs a 32-bit number as an unsigned decimal.
 * Position determined by ST7735SetTextCursor.
 * Color set by ST7735SetTextColor.
 * Variable format of 1-10 digits with no space before or after.
 * @param n 32 bit number to display.
 */
void ST7735OutUDec(uint32_t n) {
    decimalMessageIdx = 0;
    fillDecimalMessage(n);
    decimalMessage[decimalMessageIdx] = 0; // terminate end of message.

    ST7735DrawString(ST7735TextX, ST7735TextY, decimalMessage, ST7735TextColor, ST7735_BLACK);
    ST7735TextX = ST7735TextX + decimalMessageIdx;

    // Draw an asterisk at the last position if text overflows the screen.
    if (ST7735TextX > 20){
        ST7735TextX = 20;
        ST7735DrawChar(ST7735TextX*6, ST7735TextY*10, '*', ST7735_RED, ST7735_BLACK, 1);
    }
}

/**
 * ST7735OutUDec outputs a 32-bit number as an unsigned 4-digit decimal.
 * Position determined by ST7735SetTextCursor.
 * Color set by ST7735SetTextColor.
 * @param n 32 bit number to display.
 */
void ST7735OutUDec4(uint32_t n) {
    decimalMessageIdx = 0;
    fillDecimalMessage4(n);
    decimalMessage[decimalMessageIdx] = 0; // terminate end of message.

    ST7735DrawString(ST7735TextX, ST7735TextY, decimalMessage, ST7735TextColor, ST7735_BLACK);
    ST7735TextX = ST7735TextX + decimalMessageIdx;

    // Draw an asterisk at the last position if text overflows the screen.
    if (ST7735TextX > 20) {
        ST7735TextX = 20;
        ST7735DrawChar(ST7735TextX*6, ST7735TextY*10, '*', ST7735_RED, ST7735_BLACK, 1);
    }
}

/**
 * ST7735OutUDec outputs a 32-bit number as an unsigned 5-digit decimal.
 * Position determined by ST7735SetTextCursor.
 * Color set by ST7735SetTextColor.
 * @param n 32 bit number to display.
 */
void ST7735OutUDec5(uint32_t n) {
    decimalMessageIdx = 0;
    fillDecimalMessage5(n);
    decimalMessage[decimalMessageIdx] = 0; // terminate end of message.

    ST7735DrawString(ST7735TextX, ST7735TextY, decimalMessage, ST7735TextColor, ST7735_BLACK);
    ST7735TextX = ST7735TextX + decimalMessageIdx;

    // Draw an asterisk at the last position if text overflows the screen.
    if (ST7735TextX > 20) {
        ST7735TextX = 20;
        ST7735DrawChar(ST7735TextX*6, ST7735TextY*10, '*', ST7735_RED, ST7735_BLACK, 1);
    }
}

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
void ST7735FixedDecOut2(int32_t n) {
    int32_t t = n; // total number
    // if input is greater than 9999 or less than 9999
    if (n < -9999) {
        ST7735DrawStringAtCursor(" -**.**");  // output -**.** to display
        return;
    } else if(n > 9999) {
        ST7735DrawStringAtCursor("  **.**");  // output **.** to display
        return;
    } else {
        ST7735DrawStringAtCursor(" ");
        if (n < 0) {                      // if number is negative
            t = -t;                       // make t into positive number
            if (t < 100) {                // if t is greater than -1,
                ST7735DrawStringAtCursor(" -0."); // print leading space then -0.
            }else if (t < 1000) {         // if t is between -1 and -10,
                ST7735DrawStringAtCursor(" -");   // print a leading space and minus sign,
                ST7735OutUDec(t/100);     // then the first two digits
                ST7735DrawStringAtCursor(".");
            } else {                      // if t is between -99 and -1,
                ST7735DrawStringAtCursor("-");    // print a minus sign
                ST7735OutUDec(t/100);     // then the first two digits
                ST7735DrawStringAtCursor(".");
            }
        } else {
            if (t < 100) {                // if t is less than 1,
                ST7735DrawStringAtCursor("  0."); // print leading spaces then 0.
            } else if (t < 1000) {        // if t is between 1 and 10,
                ST7735DrawStringAtCursor("  ");   // print leading spaces
                ST7735OutUDec(t/100);     // then the first two digits
                ST7735DrawStringAtCursor(".");
            } else {                      // if t is between 10 and 99,
                ST7735DrawStringAtCursor(" ");    // print leading space
                ST7735OutUDec(t/100);     // then the first two digits
                ST7735DrawStringAtCursor(".");
            }
        }

        if ((t%100) < 10) {        // if remaining number is less than 0.1,
            ST7735DrawStringAtCursor("0"); // print 0 then the digit
            ST7735OutUDec(t%100);
        } else {                   // if remaining number is between 0.1 and 1,
            ST7735OutUDec(t%100);  // print the remaining two digits
        }
        return;
    }
}

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
void ST7735UBinOut6(uint32_t n) {
    int t = ((n*100) + 32) >> 6; // divide input by 64

    if (n >= 64000) { // if input is greater than 63999
        ST7735DrawStringAtCursor(" ***.**");   // output "***.**"
    } else {
        if (t < 100) {                 // if the number is less than 1,
            ST7735DrawStringAtCursor("   0."); // output leading spaces then 0
        } else if(t < 1000) {          // if number is between 1 and 10,
            ST7735DrawStringAtCursor("   ");   // output leading spaces and then number
            ST7735OutUDec(t/100);
            ST7735DrawStringAtCursor(".");
        } else if(t < 10000) {         // if number is between 10 and 100,
            ST7735DrawStringAtCursor("  ");    // output leading spaces and then number
            ST7735OutUDec(t/100);
            ST7735DrawStringAtCursor(".");
        } else {                       // if number is between 100 and 1000,
            ST7735DrawStringAtCursor(" ");     // output leading space and then number
            ST7735OutUDec(t/100);
            ST7735DrawStringAtCursor(".");
        }

        if ((t%100) < 10) {            // if remaining number is less than 0.1,
            ST7735DrawStringAtCursor("0");     // output 0 and then remaining digit
            ST7735OutUDec(t%100);
        } else {                       // if remaining number is between 0.1 and 1,
            ST7735OutUDec(t%100);      // then print out remaining two digits
        }
    }
}


/** Plotting and Graphing. */


/** Drawing bounds used between ST7735PlotInit and ST7735Plot. */
static volatile struct{
    int32_t minX;
    int32_t maxX;
    int32_t minY;
    int32_t maxY;
}plotBound;

/**
 * ST7735PlotInit sets up a scatter plot.
 * @param title ASCII string to label the plot. Null terminated.
 * @param minX Smallest X data value allowed, resolution=0.001.
 * @param maxX Largest X data value allowed, resolution=0.001.
 * @param minY Smallest Y data value allowed, resolution=0.001.
 * @param maxY Largest Y data value allowed, resolution=0.001.
 * Assumes minX < maxX; minY < maxY.
 */
void ST7735PlotInit(char *title, int32_t minX, int32_t maxX, int32_t minY, int32_t maxY) {
    ST7735FillScreen(ST7735_BLACK);
    ST7735SetTextCursor(0, 0);
    ST7735SetTextColor(ST7735_WHITE);
    ST7735DrawStringAtCursor(title);
    plotBound.minX = minX;
    plotBound.maxX = maxX;
    plotBound.minY = minY;
    plotBound.maxY = maxY;
}

/**
 * ST7735PlotInitWithoutReset sets up a scatter plot without clearing the screen.
 * @param title ASCII string to label the plot. Null terminated.
 * @param minX Smallest X data value allowed, resolution=0.001.
 * @param maxX Largest X data value allowed, resolution=0.001.
 * @param minY Smallest Y data value allowed, resolution=0.001.
 * @param maxY Largest Y data value allowed, resolution=0.001.
 * Assumes minX < maxX; minY < maxY.
 */
void ST7735PlotInitWithoutReset(char *title, int32_t minX, int32_t maxX, int32_t minY, int32_t maxY) {
    ST7735SetTextCursor(0, 0);
    ST7735SetTextColor(ST7735_WHITE);
    ST7735DrawStringAtCursor(title);
    plotBound.minX = minX;
    plotBound.maxX = maxX;
    plotBound.minY = minY;
    plotBound.maxY = maxY;
}

/**
 * ST7735Plot plots an array of (X, Y) points.
 * @param num Number of data points in each array.
 * @param bufX Array of 32-bit fixed point data, resolution=0.001.
 * @param bufY Array of 32-bit fixed point data, resolution=0.001.
 * Assumes ST7735PlotInit has been previously called and neglects points outside
 * of bounds.
 */
void ST7735Plot(uint32_t num, int32_t bufX[], int32_t bufY[]) {
    // Plot each point given their coordinates if the coordinates are valid.
    for (uint32_t i = 0; i < num; i++) {
        if (plotBound.minX <= bufX[i] &&
            bufX[i] <= plotBound.maxX &&
            plotBound.minY <= bufY[i] &&
            bufY[i] <= plotBound.maxY) {
            /* Scale ranges. Y is plus 32 since Y-coords start at 32.
             *(0 - 31 restricted for labels and messages). */
            ST7735DrawPixel(
                (127*(bufX[i] - plotBound.minX))/(plotBound.maxX - plotBound.minX),
                32 + (127*(plotBound.maxY - bufY[i]))/(plotBound.maxY - plotBound.minY),
                ST7735_WHITE);
        }
    }
}

/**
 * ST7735DrawLineGraph draws a line graph onto the display.
 * @param num Number of data points in each array.
 * @param bufX Array of 32-bit fixed point data, resolution=0.001.
 * @param bufY Array of 32-bit fixed point data, resolution=0.001.
 * @param color RGB line and point color.
 */
void ST7735DrawLineGraph(uint32_t num, int32_t bufX[], int32_t bufY[], uint16_t color) {
    // Plot each point given their coordinates if the coordinates are valid.
    for (uint32_t i = 0; i < num; i++) {
        if (plotBound.minX <= bufX[i] &&
            bufX[i] <= plotBound.maxX &&
            plotBound.minY <= bufY[i] &&
            bufY[i] <= plotBound.maxY) {
            uint32_t x = (127*(bufX[i] - plotBound.minX))/(plotBound.maxX - plotBound.minX);
            uint32_t y = 32 + (127*(plotBound.maxY - bufY[i]))/(plotBound.maxY - plotBound.minY);

            /* Scale ranges. Y is plus 32 since Y-coords start at 32.
             *(0 - 31 restricted for labels and messages). */
            ST7735DrawPixel(x, y, color);

            // Draw lines
            if (i < num -1) {
                uint32_t x2 = (127*(bufX[i + 1] - plotBound.minX))/(plotBound.maxX - plotBound.minX);
                uint32_t y2 = 32 + (127*(plotBound.maxY - bufY[i + 1]))/(plotBound.maxY - plotBound.minY);
                ST7735DrawLine(x, y, x2, y2, color);
            }
        }
    }
}
