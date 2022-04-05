/**
 * @file DMA.h
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2022-03-01
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#pragma once

#include <stdint.h>
#include <stdbool.h>

enum DMAChannel {
    DMA0,
    DMA1,
    DMA2,
    DMA3,
    DMA4,
    DMA5,
    DMA6,
    DMA7,
    DMA8,
    DMA9,
    DMA10,
    DMA11,
    DMA12,
    DMA13,
    DMA14,
    DMA15,
    DMA16,
    DMA17,
    DMA18,
    DMA19,
    DMA20,
    DMA21,
    DMA22,
    DMA23,
    DMA24,
    DMA25,
    DMA26,
    DMA27,
    DMA28,
    DMA29,
    DMA30,
    DMA31,
    DMA_COUNT
};

typedef struct DMAConfig {
    enum DMAChannel channel;

    uint8_t assignment;

    bool isHighPriority;

    bool isOnlyBurst;

} DMAConfig_t;

typedef struct DMA {
    
} DMA_t;

single request
burst request
modes
    stop
    basic
    auto
    ping pong
    scatter gather

DMA_t DMAInit(DMAConfig_t config);
// Overall uDMA configuration
    // Enable uDMA clock in RCGCDMA
    // Enable uDMA controller in DMACFG
    // Program location of channel control table
// Channel attribute configuration
    // Set priority
    // Set primary channel control structure
    // Allow uDMA to respond to single/burst request
    // Allow uDMA to recognize channel requests
// Channel control structure configuration
    // for software memory transfer
    // set source end ptr
    // set dest end ptr
    // set control word
// Enable channel
// Issue transfer request