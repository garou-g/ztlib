/*******************************************************************************
 * @file    spi.h
 * @author  garou (xgaroux@gmail.com)
 * @brief   Header file of ESP32 SPI bus driver.
 ******************************************************************************/

#pragma once

#include "periph/spi.h"
#include "periph/gpio.h"

#include "driver/spi_master.h"

namespace esp32 {

struct SpiConfig {
    spi_host_device_t spi;
    // SpiMode mode;
    // SpiPolarity polarity;
    // SpiFrame frame;
    // SpiPrescaler prescaler;
    int clk;
    int mosi;
    int miso;
};

/// @brief ESP32 SPI bus peripheral driver
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

    void readWrite(uint16_t data, bool save = true);

    SpiConfig config_;
    spi_device_handle_t spiHandle_;
    Gpio *cs_ = nullptr;
};

}; // namespace esp32

/***************************** END OF FILE ************************************/
