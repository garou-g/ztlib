/*******************************************************************************
 * @file    spi.cpp
 * @author  garou (xgaroux@gmail.com)
 * @brief   ESP32 SPI bus driver.
 ******************************************************************************/

#include "esp32/spi.h"

#include <cassert>

namespace esp32 {

static bool checkConfig(const esp32::SpiConfig* config)
{
    assert(config != nullptr);
    return true;
    // return config->spi == SPI0 || config->spi == SPI1 || config->spi == SPI2;
}

bool Spi::setConfig(const void* drvConfig)
{
    if (isOpen())
        return false;

    const esp32::SpiConfig* config = static_cast<const esp32::SpiConfig*>(drvConfig);
    if (!checkConfig(config)) {
        return false;
    }
    config_ = *config;
    return true;
}

bool Spi::open()
{
    if (isOpen())
        return false;

    if (!checkConfig(&config_)) {
        return false;
    }

    spi_bus_config_t buscfg = {
        .mosi_io_num = config_.mosi,
        .miso_io_num = config_.miso,
        .sclk_io_num = config_.clk,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz = 128,
    };
    spi_bus_initialize(config_.spi, &buscfg, SPI_DMA_CH_AUTO);

    // spi_device_interface_config_t devcfg = {
    //     .clock_speed_hz = 10 * 1000 * 1000, // Clock out at 10 MHz
    //     .mode = 0, // SPI mode 0
    //     .spics_io_num = PIN_NUM_CS, // CS pin
    //     .queue_size = 7, // We want to be able to queue 7 transactions at a time
    //     .pre_cb = lcd_spi_pre_transfer_callback, // Specify pre-transfer callback to handle D/C line
    // };
    // spi_bus_add_device(config_.spi, &devcfg, &spiHandle_);

    setOpened(true);
    return true;
}

void Spi::close()
{
    if (isOpen()) {
        setOpened(false);
    }
}

int32_t Spi::write_(const void* buf, uint32_t len)
{
    assert(buf != nullptr);

    if (!isOpen())
        return -1;

    // const uint8_t* bytes = static_cast<const uint8_t*>(buf);
    // const uint16_t* halfwords = static_cast<const uint16_t*>(buf);
    // const uint32_t* words = static_cast<const uint32_t*>(buf);
    // const int32_t reg = getReg();

    // Set chip select
    if (cs_)
        cs_->reset();

    // // Send register or command if needed
    // if (reg >= 0) {
    //     if (config_.frame == SpiFrame::Frame32Bit) {
    //         readWrite(reg >> 16, false);
    //         readWrite(reg, false);
    //     } else {
    //         readWrite(reg, false);
    //     }
    //     while (SET == spi_i2s_flag_get(config_.spi, SPI_FLAG_TRANS));
    // }

    // // Send and receive data
    // for (uint32_t i = 0; i < len; ++i) {
    //     if (config_.frame == SpiFrame::Frame32Bit) {
    //         readWrite(words[i] >> 16);
    //         readWrite(words[i]);
    //     } else if (config_.frame == SpiFrame::Frame16Bit) {
    //         readWrite(halfwords[i]);
    //     } else {
    //         readWrite(bytes[i]);
    //     }
    //     while (SET == spi_i2s_flag_get(config_.spi, SPI_FLAG_TRANS));
    // }

    // Reset chip select
    if (cs_)
        cs_->set();

    return len;
}

int32_t Spi::read_(void* buf, uint32_t len)
{
    assert(buf != nullptr);

    if (!isOpen() || len == 0)
        return -1;

    // // Get pointers to data variants
    // uint8_t* bytes = static_cast<uint8_t*>(buf);
    // uint16_t* halfwords = static_cast<uint16_t*>(buf);
    // uint32_t* words = static_cast<uint32_t*>(buf);

    // // Check sizes
    // const uint32_t currSize = rxQueue_.size();
    // const uint32_t size = currSize < len ? currSize : len;

    // // Fill data from queue buffer
    // for (uint32_t i = 0; i < size; ++i) {
    //     if (config_.frame == SpiFrame::Frame8Bit) {
    //         bytes[i] = rxQueue_.front();
    //     } else if (config_.frame == SpiFrame::Frame16Bit) {
    //         halfwords[i] = rxQueue_.front();
    //     } else {
    //         const uint16_t high = rxQueue_.front();
    //         rxQueue_.pop();
    //         words[i] = (high << 16) | (rxQueue_.front() & 0xFFFF);
    //     }
    //     rxQueue_.pop();
    // }
    // return size;

    return -1;
}

bool Spi::ioctl(uint32_t cmd, void* pValue)
{
    if (!isOpen())
        return false;

    switch (static_cast<IoctlCmd>(cmd)) {
    case kSetCs:
        if (pValue != nullptr) {
            cs_ = static_cast<Gpio*>(pValue);
            return true;
        }
        break;
    case kFlushInput:
        return true;
    default:
        break;
    }

    return false;
}

}; // namespace esp32

/***************************** END OF FILE ************************************/
