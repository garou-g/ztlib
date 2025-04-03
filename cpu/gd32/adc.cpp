/*******************************************************************************
 * @file    adc.cpp
 * @author  4lagest (zakirodion@gmail.com)
 * @brief   GD32 ADC driver.
 ******************************************************************************/

#include "gd32/adc.h"

#include <cassert>

namespace gd32 {

static bool checkConfig(const gd32::AdcConfig* config)
{
    assert(config != nullptr);
#if defined(GD32F4XX_H)
    return config->adc == ADC0 || config->adc == ADC1 || config->adc == ADC2;
#else
    return config->adc == ADC0 || config->adc == ADC1;
#endif
}

static void enableClock(uint32_t adc)
{
    rcu_periph_enum portClk;
    switch (adc) {
    case ADC0:
        portClk = RCU_ADC0;
        break;
    case ADC1:
        portClk = RCU_ADC1;
        break;
#if defined(GD32F4XX_H)
    case ADC2:
        portClk = RCU_ADC1;
        break;
#endif
    default:
        break;
    }
    rcu_periph_clock_enable(portClk);
}

static void initAdcPeriph(const gd32::AdcConfig* config)
{
    if (config->adc == 0)
        return;

    enableClock(config->adc);

#if defined(GD32F4XX_H)
    adc_clock_config(static_cast<uint32_t>(config->prescaler));
    adc_deinit();
    adc_resolution_config(config->adc, config->resolution);
    switch (config->mode) {
    case AdcMode::OneShoot:
        adc_sync_mode_config(ADC_SYNC_MODE_INDEPENDENT);
        adc_special_function_config(config->adc, ADC_CONTINUOUS_MODE, DISABLE);
        adc_special_function_config(config->adc, ADC_SCAN_MODE, DISABLE);
        adc_data_alignment_config(config->adc, ADC_DATAALIGN_RIGHT);

        adc_channel_length_config(config->adc, ADC_ROUTINE_CHANNEL, 1);
        adc_routine_channel_config(config->adc, 0, config->channel, config->sampleTime);

        adc_external_trigger_source_config(config->adc, ADC_ROUTINE_CHANNEL, ADC_EXTTRIG_ROUTINE_T0_CH0);
        adc_external_trigger_config(config->adc, ADC_ROUTINE_CHANNEL, EXTERNAL_TRIGGER_DISABLE);
        break;

    case AdcMode::Continuous:
        break;

    default:
        break;
    }
    adc_enable(config->adc);
    adc_calibration_enable(config->adc);
    for (volatile int i = 0; i < 1000; ++i)
        ; // wait for calibration to finish
#endif
}

bool Adc::setConfig(const void* drvConfig)
{
    if (isOpen())
        return false;

    const gd32::AdcConfig* config = static_cast<const AdcConfig*>(drvConfig);
    if (!checkConfig(config))
        return false;

    config_ = *config;
    return true;
}

bool Adc::open()
{
    if (isOpen() && !checkConfig(&config_))
        return false;

    initGpioPeriph(&config_.gpioConfig);
    initAdcPeriph(&config_);

    setOpened(true);
    return true;
}

void Adc::close()
{
    if (!isOpen())
        return;

    deinitGpioPeriph(&config_.gpioConfig);
    adc_disable(config_.adc);
    adc_deinit();

    setOpened(false);
}

void Adc::start()
{
    adc_software_trigger_enable(config_.adc, ADC_ROUTINE_CHANNEL);
}

uint16_t Adc::value()
{
    return adc_routine_data_read(config_.adc);
}

bool Adc::isReady()
{
    if (adc_flag_get(config_.adc, ADC_FLAG_EOC)) {
        adc_flag_clear(config_.adc, ADC_FLAG_EOC);
        return true;
    } else {
        return false;
    }
}

bool Adc::ioctl(uint32_t cmd, void* pValue)
{
    switch (static_cast<IoctlCmd>(cmd)) {
    case kSetMode:
        if (pValue != nullptr) {
            config_.mode = *(static_cast<AdcMode*>(pValue));
            return true;
        }
        break;

    case kSetResolution:
        if (pValue != nullptr) {
            config_.resolution = *(static_cast<uint32_t*>(pValue));
            return true;
        }
        break;

    case kSetSampleTime:
        if (pValue != nullptr) {
            config_.sampleTime = *(static_cast<uint32_t*>(pValue));
            return true;
        }
        break;

    default:
        break;
    }

    return false;
}

}; // namespace gd32

/***************************** END OF FILE ************************************/
