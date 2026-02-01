/*******************************************************************************
 * @file    spi.cpp
 * @author  garou (xgaroux@gmail.com)
 * @brief   ESP32 SPI bus driver.
 ******************************************************************************/

#include "esp32/spi.h"
#include "driver/gpio.h"

#include <cassert>

namespace esp32 {

static bool checkConfig(const esp32::SpiConfig* config)
{
    assert(config != nullptr);
    return config->spi == SPI1_HOST || config->spi == SPI2_HOST;
}

P_Spi::P_Spi(ISpiQueue& rx)
    : rxQueue_(rx)
{
}

bool P_Spi::setConfig(const void* drvConfig)
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

bool P_Spi::open()
{
    if (isOpen())
        return false;

    if (!checkConfig(&config_)) {
        return false;
    }

    // Init SPI instance structures
    const spi_bus_config_t buscfg = {
        .mosi_io_num = config_.mosi,
        .miso_io_num = config_.miso,
        .sclk_io_num = config_.clk,
        .quadwp_io_num = GPIO_NUM_NC,
        .quadhd_io_num = GPIO_NUM_NC,
        .data4_io_num = GPIO_NUM_NC,
        .data5_io_num = GPIO_NUM_NC,
        .data6_io_num = GPIO_NUM_NC,
        .data7_io_num = GPIO_NUM_NC,
        .max_transfer_sz = 0,
        .flags = 0,
        .intr_flags = 0,
    };

    spi_device_interface_config_t devcfg = {};
    devcfg.mode = static_cast<uint8_t>(config_.polarity);
    devcfg.clock_speed_hz = config_.speed;
    devcfg.spics_io_num = GPIO_NUM_NC;
    devcfg.queue_size = 1;

    spi_bus_initialize(config_.spi, &buscfg, SPI_DMA_CH_AUTO);
    spi_bus_add_device(config_.spi, &devcfg, &spi_);

    setOpened(true);
    return true;
}

void P_Spi::close()
{
    if (isOpen()) {
        spi_bus_remove_device(spi_);
        spi_bus_free(config_.spi);
        spi_ = nullptr;
        setOpened(false);
    }
}

int32_t P_Spi::write_(const void* buf, uint32_t len)
{
    assert(buf != nullptr);

    if (!isOpen())
        return -1;

    const int32_t reg = getReg();

    // Set chip select
    if (cs_)
        cs_->reset();

    // Send register or command if needed
    if (reg >= 0) {
        readWrite(&reg, 1, false);
    }
    readWrite(buf, len);

    // Reset chip select
    if (cs_)
        cs_->set();

    return len;
}

int32_t P_Spi::read_(void* buf, uint32_t len)
{
    assert(buf != nullptr);

    if (!isOpen() || len == 0)
        return -1;

    // Check sizes
    const uint32_t currSize = rxQueue_.size();
    const uint32_t size = currSize < len ? currSize : len;

    // Fill data from queue buffer
    for (uint32_t i = 0; i < size; ++i) {
        const auto data = rxQueue_.front();
        rxQueue_.pop();

        switch (config_.frame) {
        default:
        case SpiFrame::Frame8Bit:
            static_cast<uint8_t*>(buf)[i] = data;
            break;
        case SpiFrame::Frame16Bit:
            static_cast<uint16_t*>(buf)[i] = data;
            break;
        case SpiFrame::Frame32Bit:
            static_cast<uint32_t*>(buf)[i] = data;
            break;
        }
    }
    return size;
}

bool P_Spi::ioctl(uint32_t cmd, void* pValue)
{
    if (!isOpen())
        return false;

    switch (cmd) {
    case Spi::kSetCs:
        if (pValue != nullptr) {
            cs_ = static_cast<Gpio*>(pValue);
            return true;
        }
        break;
    case SerialDrv::kFlushInput:
        rxQueue_.clear();
        return true;
    default:
        break;
    }

    return false;
}

void P_Spi::readWrite(const void* txBuf, uint32_t len, bool save)
{
    const size_t frameLen = static_cast<size_t>(config_.frame);
    const size_t transLen = frameLen * len;
    const bool misoStageNeeded = save && config_.miso != GPIO_NUM_NC;
    uint8_t rxBuf[transLen >> 3];

    spi_transaction_t t = {};
    t.length = transLen;
    t.tx_buffer = txBuf;
    t.rx_buffer = misoStageNeeded ? rxBuf : NULL;
    esp_err_t ret = spi_device_transmit(spi_, &t);

    if (ret == ESP_OK && misoStageNeeded) {
        for (uint32_t i = 0; i < len; ++i) {
            if (rxQueue_.full())
                break;

            switch (config_.frame) {
            default:
            case SpiFrame::Frame8Bit:
                rxQueue_.push(static_cast<const uint8_t*>(t.rx_buffer)[i]);
                break;

            case SpiFrame::Frame16Bit:
                rxQueue_.push(static_cast<const uint16_t*>(t.rx_buffer)[i]);
                break;

            case SpiFrame::Frame32Bit:
                rxQueue_.push(static_cast<const uint32_t*>(t.rx_buffer)[i]);
                break;
            }
        }
    }
}

}; // namespace esp32

/***************************** END OF FILE ************************************/
