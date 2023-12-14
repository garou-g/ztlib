/*******************************************************************************
 * @file    spi.cpp
 * @author  garou (xgaroux@gmail.com)
 * @brief   GD32 SPI bus driver.
 ******************************************************************************/

#include "spi.h"
#include "gd32_types.h"

#include <cassert>

using namespace gd32;

static void initSpiGpio(const gd32::GpioConfig& conf);

bool Spi::open(const void* drvConfig)
{
    if (isOpen())
        return false;

    assert(drvConfig != nullptr);
    const gd32::SpiConfig* config
        = static_cast<const gd32::SpiConfig*>(drvConfig);
    if (config->spi == 0 || !(config->spi == SPI0 || config->spi == SPI1
        || config->spi == SPI2))
        return false;

    spi_ = config->spi;
    frame_ = config->frame;

    rcu_periph_enum spiClock;
    if (spi_ == SPI0) {
        spiClock = RCU_SPI0;
    } else if (spi_ == SPI1) {
        spiClock = RCU_SPI1;
    } else {
        spiClock = RCU_SPI2;
    }
    rcu_periph_clock_enable(spiClock);

    if (config->clk.port != 0)
        initSpiGpio(config->clk);
    if (config->mosi.port != 0)
        initSpiGpio(config->mosi);
    if (config->miso.port != 0)
        initSpiGpio(config->miso);

    spi_i2s_deinit(spi_);
    spi_parameter_struct spiParams = {
        .device_mode = config->mode == SpiMode::Master ? SPI_MASTER : SPI_SLAVE,
        .trans_mode = SPI_TRANSMODE_FULLDUPLEX,
        .frame_size = frame_ == SpiFrame::Frame8Bit
            ? SPI_FRAMESIZE_8BIT : SPI_FRAMESIZE_16BIT,
        .nss = SPI_NSS_SOFT,
        .endian = SPI_ENDIAN_MSB,
        .clock_polarity_phase = 0,
        .prescale = 0,
    };
    switch (config->polarity) {
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
    switch (config->prescaler) {
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

    spi_init(spi_, &spiParams);
    spi_enable(spi_);
    setOpened(true);

    return true;
}

void Spi::close()
{
    if (isOpen()) {
        spi_disable(spi_);
        setOpened(false);
        spi_ = -1;
        cs_ = nullptr;
    }
}

int32_t Spi::write_(const void* buf, uint32_t len)
{
    assert(buf != nullptr);

    if (!isOpen())
        return -1;

    const uint8_t* bytes = static_cast<const uint8_t*>(buf);
    const uint16_t* halfwords = static_cast<const uint16_t*>(buf);
    const uint32_t* words = static_cast<const uint32_t*>(buf);
    const int32_t reg = getReg();
    const int32_t addr = getAddr();

    for (size_t i = 0; i < len; ++i) {
        if (cs_)
            cs_->reset();

        if (frame_ == SpiFrame::Frame32Bit) {
            spi_i2s_data_transmit(spi_, (words[i] >> 16) & 0xFFFF);
            while (RESET == spi_i2s_flag_get(spi_, SPI_FLAG_TBE));
            spi_i2s_data_transmit(spi_, words[i] & 0xFFFF);
        } else if (frame_ == SpiFrame::Frame16Bit) {
            spi_i2s_data_transmit(spi_, halfwords[i]);
        } else {
            spi_i2s_data_transmit(spi_, bytes[i]);
        }
        while (SET == spi_i2s_flag_get(spi_, SPI_FLAG_TRANS));

        if (cs_)
            cs_->set();
    }

    return len;
}

int32_t Spi::read_(void* buf, uint32_t len)
{
    assert(buf != nullptr);

    if (!isOpen() || len == 0)
        return -1;

    const uint8_t* bytes = static_cast<const uint8_t*>(buf);
    const int32_t reg = getReg();
    const int32_t addr = getAddr();

    // TODO:
    return len;
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
    default:
        break;
    }

    return false;
}

static void initSpiGpio(const gd32::GpioConfig& conf)
{
    rcu_periph_clock_enable(conf.clock);
#if defined(GD32F4XX_H)
    gpio_af_set(conf.port, conf.mode, conf.pin);
    gpio_mode_set(conf.port, GPIO_MODE_AF, GPIO_PUPD_NONE, conf.pin);
    gpio_output_options_set(conf.port, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, conf.pin);
#else
    gpio_init(conf.port, conf.mode, GPIO_OSPEED_50MHZ, conf.pin);
#endif
}

/***************************** END OF FILE ************************************/
