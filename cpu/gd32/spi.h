/*******************************************************************************
 * @file    spi.h
 * @author  garou (xgaroux@gmail.com)
 * @brief   Header file of GD32 SPI bus driver.
 ******************************************************************************/

#pragma once

#include "periph/spi.h"
#include "gd32/gd32_types.h"
#include "periph/gpio.h"

namespace gd32 {

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

/// @brief GD32 SPI bus peripheral driver
class Spi : public ::Spi {
public:
    Spi() = default;

    bool setConfig(const void* drvConfig) override;
    bool open() override;
    void close() override;

    bool ioctl(uint32_t cmd, void* pValue) override;

private:
    int32_t write_(const void* buf, uint32_t len) override;
    int32_t read_(void* buf, uint32_t len) override;

    SpiConfig config_;
    Gpio *cs_ = nullptr;
};

}; // namespace gd32

/***************************** END OF FILE ************************************/
