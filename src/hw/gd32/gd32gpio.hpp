/*******************************************************************************
 * @file    gd32gpio.hpp
 * @author  garou (xgaroux@gmail.com)
 * @brief   Header file of GD32 GPIO driver.
 ******************************************************************************/

#pragma once

#include "gpio.hpp"

#include "gd32f30x.h"

class Gd32Gpio final : public Gpio {
public:
    Gd32Gpio();

    struct Config {
        rcu_periph_enum clock;
        uint32_t port;
        uint32_t pin;
        uint32_t mode;
        bool inverse;
    };

    bool open(const void* drvConfig) override;
    void close() override;

    int32_t set() override;
    int32_t reset() override;

    bool ioctl(uint32_t cmd, void* pValue) override;

private:
    bool isPortExist(uint32_t port) const;

    uint32_t port;
    int32_t pin;
};

/***************************** END OF FILE ************************************/
