/*******************************************************************************
 * @file    adc.cpp
 * @author  4lagest (zakirodion@gmail.com)
 * @brief   GD32 ADC driver.
 ******************************************************************************/

#include "gd32/adc.h"

#include <cassert>

namespace gd32 {

#define ADC_COUNT 3
#define ADC_CH_COUNT (ADC_CHANNEL_18 + 1)

struct AdcState {
    uint32_t adc;
    bool configured;
    uint8_t count;
};

/// @brief ADC state instances pointers table for configure control
static AdcState adcStates[ADC_COUNT] = { };

/// @brief ADC instances pointers table for IRQ usage
static Adc* adcInstances[ADC_CH_COUNT] = { nullptr };

/**
 * @brief Check ADC configuration validity
 *
 * @param config ADC configuration
 * @return true if configuration is valid otherwise false
 */
static bool checkConfig(const gd32::AdcConfig* config)
{
    assert(config != nullptr);
#if defined(GD32F4XX_H)
    return config->adc == ADC0 || config->adc == ADC1 || config->adc == ADC2;
#else
    return config->adc == ADC0 || config->adc == ADC1;
#endif
}

/**
 * @brief Sets ADC pointer to instances table according to chosen ADC
 *
 * @param channel ADC channel
 * @param arg ADC instance pointer
 */
static void setHandler(uint32_t channel, Adc* arg)
{
    if (channel < ADC_CH_COUNT) {
        adcInstances[channel] = arg;
    }
}

/**
 * @brief Enables ADC clock according to chosen periph
 *
 * @param adc ADC periph
 */
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

/**
 * @brief Sets IRQ new state according to chosen ADC
 *
 * @param state new IRQ state
 */
static void setIrq(bool state)
{
    IRQn_Type irqType;
#if defined(GD32F4XX_H)
    irqType = ADC_IRQn;
#else
    return;
#endif

    if (state) {
        nvic_irq_enable(irqType, 2, 0);
    } else {
        nvic_irq_disable(irqType);
    }
}

/**
 * @brief Performs ADC peripheral initialization
 *
 * @param config driver configuration
 */
static void initAdcPeriph(const gd32::AdcConfig* config)
{
    if (config->adc == 0)
        return;

    // Find existing configuration
    AdcState* adcState = nullptr;
    for (int i = 0; i < ADC_COUNT; ++i) {
        if (adcStates[i].adc == config->adc) {
            adcState = &adcStates[i];
        }
    }

    // If not found already configured - take first empty
    if (!adcState) {
        for (int i = 0; i < ADC_COUNT; ++i) {
            if (adcStates[i].adc == 0) {
                adcState = &adcStates[i];
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
            if (config->useIrq)
                adc_interrupt_enable(config->adc, ADC_INT_EOC);
            break;

        case AdcMode::Continuous:
            adc_sync_mode_config(ADC_SYNC_MODE_INDEPENDENT);
            adc_special_function_config(config->adc, ADC_CONTINUOUS_MODE, ENABLE);
            adc_special_function_config(config->adc, ADC_SCAN_MODE, DISABLE);
            adc_data_alignment_config(config->adc, ADC_DATAALIGN_RIGHT);

            adc_external_trigger_source_config(config->adc, ADC_ROUTINE_CHANNEL, ADC_EXTTRIG_ROUTINE_T0_CH0);
            adc_external_trigger_config(config->adc, ADC_ROUTINE_CHANNEL, EXTERNAL_TRIGGER_DISABLE);
            if (config->useIrq)
                adc_interrupt_enable(config->adc, ADC_INT_EOC);
            break;

        default:
            break;
        }
        adc_enable(config->adc);
        adc_calibration_enable(config->adc);
        for (volatile int i = 0; i < 1000; ++i) ; // wait for calibration to finish
#endif
    }

#if defined(GD32F4XX_H)
    // Configure selected channel
    adc_channel_length_config(config->adc, ADC_ROUTINE_CHANNEL, 1);
    adc_routine_channel_config(config->adc, 0, config->channel, config->sampleTime);
#endif
}

/**
 * @brief Performs ADC peripheral deinitialization
 *
 * @param config driver configuration
 */
static void deinitAdcPeriph(const gd32::AdcConfig* config)
{
    if (config->adc == 0)
        return;

    // Find existing configuration
    AdcState* adcState = nullptr;
    for (int i = 0; i < ADC_COUNT; ++i) {
        if (adcStates[i].adc == config->adc) {
            adcState = &adcStates[i];
        }
    }

    // Process if this ADC was configured before
    if (adcState->configured) {
        if (adcState->count > 0)
            adcState->count--;

        // Disable current channel
        // TODO: Needs to be done

        // Disable ADC if no other open channels
        if (adcState->count == 0) {
            // Release ADC instance
            adcState->adc = 0;
            adcState->configured = false;

            // Deinit ADC
            if (config->useIrq)
                adc_interrupt_enable(config->adc, ADC_INT_EOC);
            adc_disable(config->adc);
            adc_deinit();
        }
    }
}

/**
 * @brief Set current driver configuration for further usage
 *
 * @param drvConfig driver configuration
 * @return true if configuration accepted otherwise false
 */
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

/**
 * @brief Open driver with early saved configuration
 *
 * @return true if success opened otherwise false
 */
bool Adc::open()
{
    if (isOpen() && !checkConfig(&config_))
        return false;

    initGpioPeriph(&config_.gpioConfig);
    initAdcPeriph(&config_);
    if (config_.useIrq)
        setIrq(true);
    setOpened(true);

    value_ = 0;
    ready_ = false;
    return true;
}

/**
 * @brief Close driver
 */
void Adc::close()
{
    if (!isOpen())
        return;

    deinitGpioPeriph(&config_.gpioConfig);
    deinitAdcPeriph(&config_);
    setHandler(config_.channel, nullptr);
    bool chLeft = false;
    for (uint32_t i = 0; i < ADC_CH_COUNT; ++i) {
        if (adcInstances[i] != nullptr) {
            chLeft = true;
            break;
        }
    }
    if (!chLeft) {
        setIrq(false);
    }
    setOpened(false);
}

/**
 * @brief Starts ADC conversion
 */
void Adc::start()
{
    if (config_.mode == AdcMode::Continuous) {
        adc_special_function_config(config_.adc, ADC_CONTINUOUS_MODE, ENABLE);
    } else {
        adc_special_function_config(config_.adc, ADC_CONTINUOUS_MODE, DISABLE);
    }

    setHandler(config_.channel, this);
    adc_routine_channel_config(config_.adc, 0, config_.channel, config_.sampleTime);
    adc_software_trigger_enable(config_.adc, ADC_ROUTINE_CHANNEL);
    started_ = true;
}

/**
 * @brief Stops ADC conversion when continuous mode used. No affect single mode
 */
void Adc::stop()
{
    setHandler(config_.channel, nullptr);
    adc_special_function_config(config_.adc, ADC_CONTINUOUS_MODE, DISABLE);
    started_ = false;
}

/**
 * @brief Gets ADC conversion result
 *
 * @return uint16_t ADC conversion result
 */
uint16_t Adc::value()
{
    if (config_.useIrq) {
        return value_;
    } else {
        return adc_routine_data_read(config_.adc);
    }
}

/**
 * @brief Returns ready state of ADC conversion
 *
 * @return true if conversion finished otherwise false
 */
bool Adc::isReady()
{
    // When IRQ used - check ready flag, otherwise check EOC flag
    if (config_.useIrq) {
        const bool res = ready_;
        ready_ = false;
        return res;
    } else {
        if (adc_flag_get(config_.adc, ADC_FLAG_EOC)) {
            adc_flag_clear(config_.adc, ADC_FLAG_EOC);
            return true;
        } else {
            return false;
        }
    }
}

/**
 * @brief Execute chosen command on driver
 *
 * @param cmd command to execute
 * @param pValue pointer for command data
 * @return true if command executed successfully otherwise false
 */
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

/**
 * @brief ADC IRQ processing handler
 */
void Adc::irqHandler()
{
    for (uint32_t i = 0; i < ADC_COUNT; ++i) {
        uint32_t adc = adcStates[i].adc;
        if (adc && adc_interrupt_flag_get(adc, ADC_INT_FLAG_EOC) == SET) {
            adc_interrupt_flag_clear(adc, ADC_INT_FLAG_EOC);

            for (uint32_t j = 0; j < ADC_CH_COUNT; ++j) {
                if (adcInstances[j] && adcInstances[j]->config_.adc == adc) {
                    const uint16_t res = adc_routine_data_read(adc);
                    if (!adcInstances[j]->started_)
                        break;

                    adcInstances[j]->value_ = res;
                    adcInstances[j]->ready_ = true;
                    adcInstances[j]->resultCb()
                        .call_if(adcInstances[j], res);
                }
            }
        }
    }
}

#if defined(GD32F4XX_H)

extern "C" void ADC_IRQHandler()
{
    Adc::irqHandler();
}

#endif

}; // namespace gd32

/***************************** END OF FILE ************************************/
