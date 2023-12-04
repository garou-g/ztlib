/*******************************************************************************
 * @file    spi.h
 * @author  garou (xgaroux@gmail.com)
 * @brief   Header file of SPI bus driver.
 ******************************************************************************/

#pragma once

#include "serialdrv.h"
#include "gpio.h"
#include "spi_types.h"

/// @brief SPI bus peripheral driver
class Spi : public SerialDrv {
public:
    enum IoctlCmd {
        kSetCs,
    };

    Spi() = default;

    bool open(const void* drvConfig) override;
    void close() override;

    bool ioctl(uint32_t cmd, void* pValue) override;

private:
    int32_t write_(const void* buf, uint32_t len) override;
    int32_t read_(void* buf, uint32_t len) override;

    int32_t spi_ = -1;
    SpiFrame frame_ = SpiFrame::Frame8Bit;
    int32_t clk_ = -1;
    int32_t mosi_ = -1;
    int32_t miso_ = -1;
    Gpio *cs_ = nullptr;
};

/***************************** END OF FILE ************************************/
