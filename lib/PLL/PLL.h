/**
 * @file PLL.h
 * @author Jonathan Valvano (valvano@mail.utexas.edu), modified by Matthew Yu
 *         (matthewjkyu@gmail.com) 
 * @brief PLL driver.
 * @version 0.1
 * @date 2021-09-28
 * @copyright
 * With modifications from:
 *      "Embedded Systems: Introduction to ARM Cortex M Microcontrollers",
 *          ISBN: 978-1469998749, Jonathan Valvano, copyright (c) 2020
 *      "Embedded Systems: Real Time Interfacing to ARM Cortex M Microcontrollers",
 *          ISBN: 978-1463590154, Jonathan Valvano, copyright (c) 2020
 *      "Embedded Systems: Real-Time Operating Systems for ARM Cortex-M Microcontrollers",
 *          ISBN: 978-1466468863, Jonathan Valvano, copyright (c) 2020
 *      Vol 2, Program 2.10, Figure 2.37
 * 
 * Copyright 2020 by Jonathan W. Valvano, valvano@mail.utexas.edu
 * You may use, edit, run or distribute this file
 * as long as the above copyright notice remains
 * 
 * THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
 * OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 * VALVANO SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL,
 * OR CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 * For more information about my classes, my research, and my books, see
 * http://users.ece.utexas.edu/~valvano/
 */

/** General imports. */
#include <stdint.h>


/**
 * @brief PLLInit sets up the internal clock at the provided frequency.
 * 
 * @param freq Frequency to drive the TM4C.
 * @note Use the defines defined in the header file.
 */
void PLLInit(uint32_t freq);

#define BUS_80_000_MHZ      4
#define BUS_80_MHZ          4
#define BUS_66_667_MHZ      5
#define BUS_50_000_MHZ      7
#define BUS_50_MHZ          7
#define BUS_44_444_MHZ      8
#define BUS_40_000_MHZ      9
#define BUS_40_MHZ          9
#define BUS_36_364_MHZ      10
#define BUS_33_333_MHZ      11
#define BUS_30_769_MHZ      12
#define BUS_28_571_MHZ      13
#define BUS_26_667_MHZ      14
#define BUS_25_000_MHZ      15
#define BUS_25_MHZ          15
#define BUS_23_529_MHZ      16
#define BUS_22_222_MHZ      17
#define BUS_21_053_MHZ      18
#define BUS_20_000_MHZ      19
#define BUS_20_MHZ          19
#define BUS_19_048_MHZ      20
#define BUS_18_182_MHZ      21
#define BUS_17_391_MHZ      22
#define BUS_16_667_MHZ      23
#define BUS_16_000_MHZ      24
#define BUS_16_MHZ          24
#define BUS_15_385_MHZ      25
#define BUS_14_815_MHZ      26
#define BUS_14_286_MHZ      27
#define BUS_13_793_MHZ      28
#define BUS_13_333_MHZ      29
#define BUS_12_903_MHZ      30
#define BUS_12_500_MHZ      31
#define BUS_12_121_MHZ      32
#define BUS_11_765_MHZ      33
#define BUS_11_429_MHZ      34
#define BUS_11_111_MHZ      35
#define BUS_10_811_MHZ      36
#define BUS_10_526_MHZ      37
#define BUS_10_256_MHZ      38
#define BUS_10_000_MHZ      39
#define BUS_10_MHZ          39
#define BUS_9_756_MHZ       40
#define BUS_9_524_MHZ       41
#define BUS_9_302_MHZ       42
#define BUS_9_091_MHZ       43
#define BUS_8_889_MHZ       44
#define BUS_8_696_MHZ       45
#define BUS_8_511_MHZ       46
#define BUS_8_333_MHZ       47
#define BUS_8_163_MHZ       48
#define BUS_8_000_MHZ       49
#define BUS_8_MHZ           49
#define BUS_7_843_MHZ       50
#define BUS_7_692_MHZ       51
#define BUS_7_547_MHZ       52
#define BUS_7_407_MHZ       53
#define BUS_7_273_MHZ       54
#define BUS_7_143_MHZ       55
#define BUS_7_018_MHZ       56
#define BUS_6_897_MHZ       57
#define BUS_6_780_MHZ       58
#define BUS_6_667_MHZ       59
#define BUS_6_557_MHZ       60
#define BUS_6_452_MHZ       61
#define BUS_6_349_MHZ       62
#define BUS_6_250_MHZ       63
#define BUS_6_154_MHZ       64
#define BUS_6_061_MHZ       65
#define BUS_5_970_MHZ       66
#define BUS_5_882_MHZ       67
#define BUS_5_797_MHZ       68
#define BUS_5_714_MHZ       69
#define BUS_5_634_MHZ       70
#define BUS_5_556_MHZ       71
#define BUS_5_479_MHZ       72
#define BUS_5_405_MHZ       73
#define BUS_5_333_MHZ       74
#define BUS_5_263_MHZ       75
#define BUS_5_195_MHZ       76
#define BUS_5_128_MHZ       77
#define BUS_5_063_MHZ       78
#define BUS_5_000_MHZ       79
#define BUS_4_938_MHZ       80
#define BUS_4_878_MHZ       81
#define BUS_4_819_MHZ       82
#define BUS_4_762_MHZ       83
#define BUS_4_706_MHZ       84
#define BUS_4_651_MHZ       85
#define BUS_4_598_MHZ       86
#define BUS_4_545_MHZ       87
#define BUS_4_494_MHZ       88
#define BUS_4_444_MHZ       89
#define BUS_4_396_MHZ       90
#define BUS_4_348_MHZ       91
#define BUS_4_301_MHZ       92
#define BUS_4_255_MHZ       93
#define BUS_4_211_MHZ       94
#define BUS_4_167_MHZ       95
#define BUS_4_124_MHZ       96
#define BUS_4_082_MHZ       97
#define BUS_4_040_MHZ       98
#define BUS_4_000_MHZ       99
#define BUS_4_MHZ           99
#define BUS_3_960_MHZ       100
#define BUS_3_922_MHZ       101
#define BUS_3_883_MHZ       102
#define BUS_3_846_MHZ       103
#define BUS_3_810_MHZ       104
#define BUS_3_774_MHZ       105
#define BUS_3_738_MHZ       106
#define BUS_3_704_MHZ       107
#define BUS_3_670_MHZ       108
#define BUS_3_636_MHZ       109
#define BUS_3_604_MHZ       110
#define BUS_3_571_MHZ       111
#define BUS_3_540_MHZ       112
#define BUS_3_509_MHZ       113
#define BUS_3_478_MHZ       114
#define BUS_3_448_MHZ       115
#define BUS_3_419_MHZ       116
#define BUS_3_390_MHZ       117
#define BUS_3_361_MHZ       118
#define BUS_3_333_MHZ       119
#define BUS_3_306_MHZ       120
#define BUS_3_279_MHZ       121
#define BUS_3_252_MHZ       122
#define BUS_3_226_MHZ       123
#define BUS_3_200_MHZ       124
#define BUS_3_175_MHZ       125
#define BUS_3_150_MHZ       126
#define BUS_3_125_MHZ       127




