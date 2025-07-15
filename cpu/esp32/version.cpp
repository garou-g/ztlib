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
 * @brief Read HW version and return it
 *
 * @return Version::Hardware structure with HW version
 */
Version::Hardware Version::getHwValue()
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

    // Decrease by 40 mV because ESP ADC measurement overstates
    const int major = esp_adc_cal_raw_to_voltage(hwMajor, &adc_chars) - 40;
    const int minor = esp_adc_cal_raw_to_voltage(hwMinor, &adc_chars) - 40;
    Version::Hardware hw = { 0, 0 };

    // Parse major board version
    if (major < 200) {
        hw.major = 0;
    } else if (major >= 200 && major < 400) {
        hw.major = 1;
    } else if (major >= 400 && major < 600) {
        hw.major = 2;
    } else if (major >= 600 && major < 800) {
        hw.major = 3;
    } else if (major >= 800 && major < 1000) {
        hw.major = 4;
    } else if (major >= 1000 && major < 1200) {
        hw.major = 5;
    } else if (major >= 1200 && major < 1400) {
        hw.major = 6;
    } else if (major >= 1400 && major < 1650) {
        hw.major = 7;
    } else if (major >= 1650 && major < 1850) {
        hw.major = 8;
    } else if (major >= 1850 && major < 2050) {
        hw.major = 9;
    } else if (major >= 2050 && major < 2250) {
        hw.major = 10;
    } else if (major >= 2250 && major < 2450) {
        hw.major = 11;
    } else if (major >= 2450 && major < 2650) {
        hw.major = 12;
    } else if (major >= 2650 && major < 2850) {
        hw.major = 13;
    } else if (major >= 2850 && major < 3050) {
        hw.major = 14;
    } else if (major >= 3050) {
        hw.major = 15;
    }

    // Parse minor board version
    if (minor < 200) {
        hw.minor = 0;
    } else if (minor >= 200 && minor < 400) {
        hw.minor = 1;
    } else if (minor >= 400 && minor < 600) {
        hw.minor = 2;
    } else if (minor >= 600 && minor < 800) {
        hw.minor = 3;
    } else if (minor >= 800 && minor < 1000) {
        hw.minor = 4;
    } else if (minor >= 1000 && minor < 1200) {
        hw.minor = 5;
    } else if (minor >= 1200 && minor < 1400) {
        hw.minor = 6;
    } else if (minor >= 1400 && minor < 1650) {
        hw.minor = 7;
    } else if (minor >= 1650 && minor < 1850) {
        hw.minor = 8;
    } else if (minor >= 1850 && minor < 2050) {
        hw.minor = 9;
    } else if (minor >= 2050 && minor < 2250) {
        hw.minor = 10;
    } else if (minor >= 2250 && minor < 2450) {
        hw.minor = 11;
    } else if (minor >= 2450 && minor < 2650) {
        hw.minor = 12;
    } else if (minor >= 2650 && minor < 2850) {
        hw.minor = 13;
    } else if (minor >= 2850 && minor < 3050) {
        hw.minor = 14;
    } else if (minor >= 3050) {
        hw.minor = 15;
    }
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
