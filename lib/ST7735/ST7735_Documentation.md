# ST7735

160 x 128 TFT Display w/ uSD Card Breakout

---

## Maintainers

This library was first developed by [Jonathan Valvano](http://users.ece.utexas.edu/~valvano/) for the TM4C. His original work accompanies the following readings:

- "Embedded Systems: Introduction to ARM Cortex M Microcontrollers", ISBN: 978-1469998749, Jonathan Valvano, copyright (c) 2020
- "Embedded Systems: Real Time Interfacing to ARM Cortex M Microcontrollers", ISBN: 978-1463590154, Jonathan Valvano, copyright (c) 2020
- "Embedded Systems: Real-Time Operating Systems for ARM Cortex-M Microcontrollers", ISBN: 978-1466468863, Jonathan Valvano, copyright (c) 2020

Augmentations and modifications are by Matthew Yu (2021).

---

## Introduction

This is a library for the Adafruit 1.8" SPI display.
This library works with the [Adafruit 1.8" TFT Breakout w/SD card](http://www.adafruit.com/products/358) as well as the [Adafruit raw 1.8" TFT display](http://www.adafruit.com/products/618).

---

## Hardware Connections

### ST7735 TFT and SDC

| Label        | Pin | Connected To                                            |
|--------------|-----|---------------------------------------------------------|
| Backlight    | 10  | Connected to 3.3V                                       |
| MISO         | 9   | Unconnected                                             |
| SCK          | 8   | Connected to PA2 (SSI0Clk)                              |
| MOSI         | 7   | Connected to PA5 (SSI0Tx)                               |
| TFT_CS       | 6   | Connected to PA3 (SSI0Fss)                              |
| CARD_CS      | 5   | Unconnected                                             |
| Data/Command | 4   | Connected to PA6 (GPIO), high for data, low for command |
| RESET        | 3   | Connected to PA7 (GPIO)                                 |
| VCC          | 2   | Connected to 3.3V                                       |
| Gnd          | 1   | Connected to ground                                     |

---

### HiLetgo ST7735 TFT and SDC (SDC not tested)

| Label | Pin     | Connected To                                            |
|-------|---------|---------------------------------------------------------|
| LED-  | 16      | Connected to ground                                     |
| LED+  | 15      | Connected to 3.3V                                       |
| SD_CS | 14      | Unconnected, SDC only                                   |
| MOSI  | 13      | Unconnected, SDC only                                   |
| MISO  | 12      | Unconnected, SDC only                                   |
| SCK   | 11      | Unconnected, SDC only                                   |
| CS    | 10      | Connected to PA3 (SSI0Fss)                              |
| SCL   | 9       | Connected to PA2 (SSI0Clk)                              |
| SDA   | 8       | Connected to PA5 (SSI0Tx)                               |
| A0    | 7       | Connected to PA6 (GPIO), high for data, low for command |
| RESET | 6       | Connected to PA7 (GPIO)                                 |
| * NC  | 3, 4, 5 | Unconnected                                             |
| * VCC | 2       | Connected to 3.3V                                       |
| * GND | 1       | Connected to ground                                     |