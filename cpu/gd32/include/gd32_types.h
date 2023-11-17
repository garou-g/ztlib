/*******************************************************************************
 * @file    gd32_types.h
 * @author  garou (xgaroux@gmail.com)
 * @brief   Header file of GD32 specific types.
 ******************************************************************************/

#pragma once

#include "gd32.h"

#include <stdint.h>

struct GpioConfig {
    rcu_periph_enum clock;
    uint32_t port;
    uint32_t pin;
    uint32_t mode;
    bool inverse;
};

enum class I2cMode {
    kMaster,
    kSlave,
};

struct I2cConfig {
    uint32_t i2c;
    I2cMode mode;
    GpioConfig scl;
    GpioConfig sda;
    int32_t speed;
};

struct UartConfig {
    uint32_t uart;
    GpioConfig tx;
    GpioConfig rx;
    int32_t baudrate;
};
