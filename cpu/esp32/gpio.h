/*******************************************************************************
 * @file    gpio.h
 * @author  garou (xgaroux@gmail.com)
 * @brief   Header file of ESP32 GPIO driver.
 ******************************************************************************/

#pragma once

#include "periph/gpio.h"

namespace esp32 {

/// @brief ESP32 GPIO peripheral driver
class Gpio : public ::Gpio {
public:
    Gpio() = default;

    bool setConfig(const void* drvConfig) override;
    bool open() override;
    void close() override;

    bool ioctl(uint32_t cmd, void* pValue) override;

    void set() override;
    void reset() override;
    bool get() const override;

private:
    GpioConfig config_;
};

}; // namespace esp32

/***************************** END OF FILE ************************************/
