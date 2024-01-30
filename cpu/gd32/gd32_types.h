/*******************************************************************************
 * @file    gd32_types.h
 * @author  garou (xgaroux@gmail.com)
 * @brief   Header file of GD32 specific types.
 ******************************************************************************/

#pragma once

#include "gd32/gd32.h"

#include <stdint.h>

namespace gd32 {

struct GpioConfig {
    uint32_t port;
    uint32_t pin;
    uint32_t mode;
    uint32_t function;
    uint32_t pull;
    uint32_t outputType;
    bool initHigh;
};

void initGpioPeriph(const gd32::GpioConfig* conf);
void deinitGpioPeriph(const gd32::GpioConfig* conf);

}; // namespace gd32
