/*******************************************************************************
 * @file    adc.cpp
 * @author  4lagest (zakirodion@gmail.com)
 * @brief   GD32 ADC driver.
 ******************************************************************************/

#include "gd32/adc.h"

#include <cassert>

namespace gd32 {

struct AdcConfState {
    uint32_t adc;
    bool configured;
    uint8_t count;
};

#define ADC_COUNT 3

static AdcConfState adcInstances[ADC_COUNT] = { };

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

    // Find existing configuration
    AdcConfState* adcState = nullptr;
    for (int i = 0; i < ADC_COUNT; ++i) {
        if (adcInstances[i].adc == config->adc) {
            adcState = &adcInstances[i];
        }
    }

    // If not found already configured - take first empty
    if (!adcState) {
        for (int i = 0; i < ADC_COUNT; ++i) {
            if (adcInstances[i].adc == 0) {
                adcState = &adcInstances[i];
                break;
            }
        }
        adcState->adc = config->adc;
        adcState->configured = false;
        adcState->count = 1;
    }

    // Configure ADC itself
    if (!adcState->configured) {
        adcState->configured = true;
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

    // Configure selected channel
    #if defined(GD32F4XX_H)
    adc_channel_length_config(config->adc, ADC_ROUTINE_CHANNEL, 1);
    adc_routine_channel_config(config->adc, 0, config->channel, config->sampleTime);
    #endif
}

static void deinitAdcPeriph(const gd32::AdcConfig* config)
{
    if (config->adc == 0)
        return;

    // Find existing configuration
    AdcConfState* adcState = nullptr;
    for (int i = 0; i < ADC_COUNT; ++i) {
        if (adcInstances[i].adc == config->adc) {
            adcState = &adcInstances[i];
        }
    }

    // Process if this ADC was configured before
    if (adcState->configured) {
        if (adcState->count > 0)
            adcState->count--;

        // Disable current channel
        // TODO:

        // Disable ADC if no other open channels
        if (adcState->count == 0) {
            // Release ADC instance
            adcState->adc = 0;
            adcState->configured = false;

            // Deinit ADC
            adc_disable(config->adc);
            adc_deinit();
        }
    }
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
    deinitAdcPeriph(&config_);

    setOpened(false);
}

void Adc::start()
{
    adc_routine_channel_config(config_.adc, 0, config_.channel, config_.sampleTime);
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
