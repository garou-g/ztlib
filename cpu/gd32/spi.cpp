/*******************************************************************************
 * @file    spi.cpp
 * @author  garou (xgaroux@gmail.com)
 * @brief   GD32 SPI bus driver.
 ******************************************************************************/

#include "gd32/spi.h"

#include <cassert>

namespace gd32 {

static bool checkConfig(const gd32::SpiConfig* config)
{
    assert(config != nullptr);
    return config->spi == SPI0 || config->spi == SPI1 || config->spi == SPI2;
}

P_Spi::P_Spi(ISpiQueue& rx)
    : rxQueue_(rx)
{
}

bool P_Spi::setConfig(const void* drvConfig)
{
    if (isOpen())
        return false;

    const gd32::SpiConfig* config = static_cast<const gd32::SpiConfig*>(drvConfig);
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

    rcu_periph_enum spiClock;
    if (config_.spi == SPI0) {
        spiClock = RCU_SPI0;
    } else if (config_.spi == SPI1) {
        spiClock = RCU_SPI1;
    } else {
        spiClock = RCU_SPI2;
    }
    rcu_periph_clock_enable(spiClock);

    initGpioPeriph(&config_.clk);
    initGpioPeriph(&config_.mosi);
    initGpioPeriph(&config_.miso);

    spi_i2s_deinit(config_.spi);
    spi_parameter_struct spiParams = {
        .device_mode = config_.mode == SpiMode::Master ? SPI_MASTER : SPI_SLAVE,
        .trans_mode = SPI_TRANSMODE_FULLDUPLEX,
        .frame_size = config_.frame == SpiFrame::Frame8Bit
            ? SPI_FRAMESIZE_8BIT : SPI_FRAMESIZE_16BIT,
        .nss = SPI_NSS_SOFT,
        .endian = SPI_ENDIAN_MSB,
        .clock_polarity_phase = 0,
        .prescale = 0,
    };
    switch (config_.polarity) {
    default:
    case SpiPolarity::Low1Edge:
        spiParams.clock_polarity_phase = SPI_CK_PL_LOW_PH_1EDGE; break;
    case SpiPolarity::High1Edge:
        spiParams.clock_polarity_phase = SPI_CK_PL_HIGH_PH_1EDGE; break;
    case SpiPolarity::Low2Edge:
        spiParams.clock_polarity_phase = SPI_CK_PL_LOW_PH_2EDGE; break;
    case SpiPolarity::High2Edge:
        spiParams.clock_polarity_phase = SPI_CK_PL_HIGH_PH_2EDGE; break;
    }
    switch (config_.prescaler) {
    default:
    case SpiPrescaler::Div2:
        spiParams.prescale = SPI_PSC_2; break;
    case SpiPrescaler::Div4:
        spiParams.prescale = SPI_PSC_4; break;
    case SpiPrescaler::Div8:
        spiParams.prescale = SPI_PSC_8; break;
    case SpiPrescaler::Div16:
        spiParams.prescale = SPI_PSC_16; break;
    case SpiPrescaler::Div32:
        spiParams.prescale = SPI_PSC_32; break;
    case SpiPrescaler::Div64:
        spiParams.prescale = SPI_PSC_64; break;
    case SpiPrescaler::Div128:
        spiParams.prescale = SPI_PSC_128; break;
    case SpiPrescaler::Div256:
        spiParams.prescale = SPI_PSC_256; break;
    }

    spi_init(config_.spi, &spiParams);
    spi_enable(config_.spi);
    setOpened(true);
    return true;
}

void P_Spi::close()
{
    if (isOpen()) {
        spi_disable(config_.spi);
        deinitGpioPeriph(&config_.clk);
        deinitGpioPeriph(&config_.mosi);
        deinitGpioPeriph(&config_.miso);
        cs_ = nullptr;
        setOpened(false);
    }
}

int32_t P_Spi::write_(const void* buf, uint32_t len)
{
    assert(buf != nullptr);

    if (!isOpen())
        return -1;

    const uint8_t* bytes = static_cast<const uint8_t*>(buf);
    const uint16_t* halfwords = static_cast<const uint16_t*>(buf);
    const uint32_t* words = static_cast<const uint32_t*>(buf);
    const int32_t reg = getReg();

    // Set chip select
    if (cs_)
        cs_->reset();

    // Send register or command if needed
    if (reg >= 0) {
        if (config_.frame == SpiFrame::Frame32Bit) {
            readWrite(reg >> 16, false);
            readWrite(reg, false);
        } else {
            readWrite(reg, false);
        }
        while (SET == spi_i2s_flag_get(config_.spi, SPI_FLAG_TRANS));
    }

    // Send and receive data
    for (uint32_t i = 0; i < len; ++i) {
        if (config_.frame == SpiFrame::Frame32Bit) {
            readWrite(words[i] >> 16);
            readWrite(words[i]);
        } else if (config_.frame == SpiFrame::Frame16Bit) {
            readWrite(halfwords[i]);
        } else {
            readWrite(bytes[i]);
        }
        while (SET == spi_i2s_flag_get(config_.spi, SPI_FLAG_TRANS));
    }

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

    // Get pointers to data variants
    uint8_t* bytes = static_cast<uint8_t*>(buf);
    uint16_t* halfwords = static_cast<uint16_t*>(buf);
    uint32_t* words = static_cast<uint32_t*>(buf);

    // Check sizes
    const uint32_t currSize = rxQueue_.size();
    const uint32_t size = currSize < len ? currSize : len;

    // Fill data from queue buffer
    for (uint32_t i = 0; i < size; ++i) {
        if (config_.frame == SpiFrame::Frame8Bit) {
            bytes[i] = rxQueue_.front();
        } else if (config_.frame == SpiFrame::Frame16Bit) {
            halfwords[i] = rxQueue_.front();
        } else {
            const uint16_t high = rxQueue_.front();
            rxQueue_.pop();
            words[i] = (high << 16) | (rxQueue_.front() & 0xFFFF);
        }
        rxQueue_.pop();
    }
    return size;
}

bool P_Spi::ioctl(uint32_t cmd, void* pValue)
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
    default:
        break;
    }

    return false;
}

void P_Spi::readWrite(uint16_t data, bool save)
{
    spi_i2s_data_transmit(config_.spi, data);
    if (config_.miso.port != 0) {
        while (RESET == spi_i2s_flag_get(config_.spi, SPI_FLAG_RBNE));
        const uint16_t word = spi_i2s_data_receive(config_.spi);
        if (save && !rxQueue_.full()) {
            rxQueue_.push(word);
        }
    } else {
        while (RESET == spi_i2s_flag_get(config_.spi, SPI_FLAG_TBE));
    }
}

}; // namespace gd32

/***************************** END OF FILE ************************************/
