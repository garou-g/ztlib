/*******************************************************************************
 * @file    espgpio.hpp
 * @author  garou (xgaroux@gmail.com)
 * @brief   Header file of ESP32 GPIO driver.
 ******************************************************************************/

#pragma once

#include "gpio.hpp"

#include "driver/gpio.h"

class EspGpio final : public Gpio {
public:
    EspGpio();

    bool open(const void* drvConfig) override;
    void close() override;

    int32_t set() override;
    int32_t reset() override;

    bool ioctl(uint32_t cmd, void* pValue) override;

private:
    gpio_num_t pin;
};

/***************************** END OF FILE ************************************/
