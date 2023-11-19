/*******************************************************************************
 * @file    version.cpp
 * @author  garou (xgaroux@gmail.com)
 * @brief   ESP32 version ADC driver module.
 ******************************************************************************/

#include "version.h"

#include "driver/adc.h"
#include "esp_adc_cal.h"
#include "esp_ota_ops.h"

/**
 * @brief Read HW version voltages and return it
 *
 * @return Version::HwVolt structure with HW voltages
 */
Version::HwVolt Version::getHwValue()
{
    const int NO_OF_SAMPLES = 64;
    const int DEFAULT_VREF = 1100;

    // Setup ADC and calibration parameters
    adc1_config_width(ADC_WIDTH_BIT_12);
    adc1_config_channel_atten(ADC1_CHANNEL_0, ADC_ATTEN_DB_11);
    adc1_config_channel_atten(ADC1_CHANNEL_3, ADC_ATTEN_DB_11);
    esp_adc_cal_characteristics_t adc_chars;
    esp_adc_cal_characterize(ADC_UNIT_1, ADC_ATTEN_DB_11,
        ADC_WIDTH_BIT_12, DEFAULT_VREF, &adc_chars);

    // Read ADC values and transform it to voltage
    uint32_t hwMajor = 0, hwMinor = 0;
    for (int i = 0; i < NO_OF_SAMPLES; i++) {
        hwMajor += adc1_get_raw(ADC1_CHANNEL_0);
        hwMinor += adc1_get_raw(ADC1_CHANNEL_3);
    }
    hwMajor /= NO_OF_SAMPLES;
    hwMinor /= NO_OF_SAMPLES;
    HwVolt hw;

    // Decrease by 40 mV because ESP ADC measurement overstates
    hw.major = esp_adc_cal_raw_to_voltage(hwMajor, &adc_chars) - 40;
    hw.minor = esp_adc_cal_raw_to_voltage(hwMinor, &adc_chars) - 40;
    return hw;
}

/**
 * @brief Read FW version data and return it
 *
 * @return Version::FwString structure with FW version
 */
Version::FwString Version::getFwValue()
{
    const esp_app_desc_t* app = esp_ota_get_app_description();
    const size_t size = sizeof(app->version) < FW_SIZE ?
        sizeof(app->version) : FW_SIZE;

    FwString fw;
    for (size_t i = 0; i < size; ++i)
        fw.data[i] = app->version[i];
    return fw;
}

/***************************** END OF FILE ************************************/
