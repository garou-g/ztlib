/*******************************************************************************
 * @file    spi.h
 * @author  garou (xgaroux@gmail.com)
 * @brief   Header file of ESP32 SPI bus driver.
 ******************************************************************************/

#pragma once

#include "periph/spi.h"
#include "periph/gpio.h"
#include "etl/queue.h"

#include "driver/spi_master.h"

namespace esp32 {

struct SpiConfig {
    spi_host_device_t spi;
    SpiMode mode;
    SpiPolarity polarity;
    SpiFrame frame;
    int speed;
    int clk;
    int mosi;
    int miso;
};

/// @brief Private realization of ESP32 SPI bus peripheral driver
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

    void readWrite(const void* txBuf, uint32_t len, bool save = true);

    SpiConfig config_;
    spi_device_handle_t spi_;
    ISpiQueue& rxQueue_;
    Gpio *cs_ = nullptr;
};

/// @brief ESP32 SPI bus peripheral driver
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

}; // namespace esp32

/***************************** END OF FILE ************************************/
