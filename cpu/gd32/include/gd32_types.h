/*******************************************************************************
 * @file    gd32_types.h
 * @author  garou (xgaroux@gmail.com)
 * @brief   Header file of GD32 specific types.
 ******************************************************************************/

#pragma once

#include "gd32.h"

#include <stdint.h>

namespace gd32 {

struct GpioConfig {
    uint32_t port;
    uint32_t pin;
    uint32_t mode;
    uint32_t function;
    uint32_t pull;
    uint32_t outputType;
};

void initGpioPeriph(const gd32::GpioConfig* conf);

enum class I2cMode {
    Master,
    Slave,
};

struct I2cConfig {
    uint32_t i2c;
    I2cMode mode;
    GpioConfig scl;
    GpioConfig sda;
    int32_t speed;
};

#include "spi_types.h"

enum class SpiPrescaler {
    Div2 = 0,
    Div4 = 1,
    Div8 = 2,
    Div16 = 3,
    Div32 = 4,
    Div64 = 5,
    Div128 = 6,
    Div256 = 7,
};

struct SpiConfig {
    uint32_t spi;
    SpiMode mode;
    SpiPolarity polarity;
    SpiFrame frame;
    SpiPrescaler prescaler;
    GpioConfig clk;
    GpioConfig mosi;
    GpioConfig miso;
};

struct UartConfig {
    void* baseConf;
    GpioConfig tx;
    GpioConfig rx;
};

}; // namespace gd32
