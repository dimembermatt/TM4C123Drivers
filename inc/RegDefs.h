/**
 * RegDefs.h
 * Devices: LM4F120; TM4C123
 * Description: TM4C123GH6PM Register definitions for drivers.
 * Authors: Matthew Yu.
 * Last Modified: 03/06/21
 **/
#pragma once

/**
 * See Table 11-12. Timers Register Map on p. 726 of the TM4C Datasheet for
 * more details.
 */
#define GPTM_BASE           0x40030000
#define GPTMCFG_OFFSET      0x000
#define GPTMTAMR_OFFSET     0x004
#define GPTMTBMR_OFFSET     0x008
#define GPTMCTL_OFFSET      0x00C
#define GPTMSYNC_OFFSET     0x010
#define GPTMIMR_OFFSET      0x018
#define GPTMRIS_OFFSET      0x01C
#define GPTMMIS_OFFSET      0x020
#define GPTMICR_OFFSET      0x024
#define GPTMTAILR_OFFSET    0x028
#define GPTMTBILR_OFFSET    0x02C
#define GPTMTAMATCHR_OFFSET 0x030
#define GPTMTBMATCHR_OFFSET 0x034
#define GPTMTAPR_OFFSET     0x038
#define GPTMTBPR_OFFSET     0x03C
#define GPTMTAPMR_OFFSET    0x040
#define GPTMTBPMR_OFFSET    0x044
#define GPTMTAR_OFFSET      0x048
#define GPTMTBR_OFFSET      0x04C
#define GPTMTAV_OFFSET      0x050
#define GPTMTBV_OFFSET      0x054
#define GPTMRTCPD_OFFSET    0x058
#define GPTMTAPS_OFFSET     0x05C
#define GPTMTBPS_OFFSET     0x060
#define GPTMTAPV_OFFSET     0x064
#define GPTMTBPV_OFFSET     0x068
#define GPTMPP_OFFSET       0xFC0

/** 
 * See Table 3-8. Peripherals Register Map on p, 134 of the TM4C Datasheet for
 * more details.
 */
#define PERIPHERALS_BASE    0xE000E000
#define NVIC_EN0_OFFSET     0x100
#define NVIC_EN1_OFFSET     0x104
#define NVIC_EN2_OFFSET     0x108
#define NVIC_EN3_OFFSET     0x10C
#define NVIC_EN4_OFFSET     0x110
#define NVIC_PRI0_OFFSET    0x400
#define NVIC_PRI1_OFFSET    0x404
#define NVIC_PRI2_OFFSET    0x408
#define NVIC_PRI3_OFFSET    0x40C
#define NVIC_PRI4_OFFSET    0x410
#define NVIC_PRI5_OFFSET    0x414
#define NVIC_PRI6_OFFSET    0x418
#define NVIC_PRI7_OFFSET    0x41C
#define NVIC_PRI8_OFFSET    0x420
#define NVIC_PRI9_OFFSET    0x424
#define NVIC_PRI10_OFFSET   0x428
#define NVIC_PRI11_OFFSET   0x42C
#define NVIC_PRI12_OFFSET   0x430
#define NVIC_PRI13_OFFSET   0x434
#define NVIC_PRI14_OFFSET   0x438
#define NVIC_PRI15_OFFSET   0x43C
#define NVIC_PRI16_OFFSET   0x440
#define NVIC_PRI17_OFFSET   0x444
#define NVIC_PRI18_OFFSET   0x448
#define NVIC_PRI19_OFFSET   0x44C
#define NVIC_PRI20_OFFSET   0x450
#define NVIC_PRI21_OFFSET   0x454
#define NVIC_PRI22_OFFSET   0x458
#define NVIC_PRI23_OFFSET   0x45C
#define NVIC_PRI24_OFFSET   0x460
#define NVIC_PRI25_OFFSET   0x464
#define NVIC_PRI26_OFFSET   0x468
#define NVIC_PRI27_OFFSET   0x46C
#define NVIC_PRI28_OFFSET   0x470
#define NVIC_PRI29_OFFSET   0x474
#define NVIC_PRI30_OFFSET   0x478
#define NVIC_PRI31_OFFSET   0x47C
#define NVIC_PRI32_OFFSET   0x480
#define NVIC_PRI33_OFFSET   0x484
#define NVIC_PRI34_OFFSET   0x488
