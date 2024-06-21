/*******************************************************************************
 * @file    spi.h
 * @author  garou (xgaroux@gmail.com)
 * @brief   Header file of GD32 SPI bus driver.
 ******************************************************************************/

#pragma once

#include "periph/spi.h"
#include "gd32/gd32_types.h"
#include "periph/gpio.h"
#include "etl/queue.h"

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

/// @brief Private realization of GD32 SPI bus peripheral driver
class P_Spi : public ::Spi {
public:
    using ISpiQueue = etl::iqueue<uint32_t, etl::memory_model::MEMORY_MODEL_MEDIUM>;

    P_Spi(ISpiQueue& rx);

    bool setConfig(const void* drvConfig) override;
    bool open() override;
    void close() override;

    bool ioctl(uint32_t cmd, void* pValue) override;

private:
    int32_t write_(const void* buf, uint32_t len) override;
    int32_t read_(void* buf, uint32_t len) override;

    void readWrite(uint16_t data, bool save = true);

    SpiConfig config_;
    ISpiQueue& rxQueue_;
    Gpio *cs_ = nullptr;
};

/// @brief GD32 SPI bus peripheral driver
template <size_t SIZE>
class Spi final : public P_Spi {
public:
    using SpiQueue = etl::queue<uint32_t, SIZE, etl::memory_model::MEMORY_MODEL_MEDIUM>;

    Spi()
        : P_Spi(rxQueue_)
    {
    }

private:
    SpiQueue rxQueue_;
};

}; // namespace gd32

/***************************** END OF FILE ************************************/
