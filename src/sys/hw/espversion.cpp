/*******************************************************************************
 * @file    espversion.cpp
 * @author  garou (xgaroux@gmail.com)
 * @brief   ESP32 version ADC driver module.
 ******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "espversion.hpp"

#include "driver/adc.h"
#include "esp_adc_cal.h"
#include "esp_ota_ops.h"
#include "nvs_flash.h"
#include "esp_log.h"

#define TAG "ver"

/* Private typedef -----------------------------------------------------------*/
/* Private defines -----------------------------------------------------------*/
/* Private macros ------------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/

/**
 * @brief Construct a new version driver instance
 */
EspVersion::EspVersion()
{
    // One time NVS flash init
    esp_err_t nvsErr = nvs_flash_init();
    if (nvsErr == ESP_ERR_NVS_NO_FREE_PAGES ||
        nvsErr == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        // NVS partition was truncated and needs to be erased
        // Retry nvs_flash_init
        nvs_flash_erase();
        nvs_flash_init();
    }
}

/* Private functions ---------------------------------------------------------*/

/**
 * @brief Read HW version voltages and return it
 *
 * @return Version::HwVolt structure with HW voltages
 */
Version::HwVolt EspVersion::getHwValue()
{
    nvs_handle_t handle;
    const esp_err_t openErr = nvs_open("device", NVS_READWRITE, &handle);
    ESP_LOGI(TAG, "nvs_open device %d", openErr);

    HwVolt hw;
    if (openErr == ESP_OK) {
        size_t size = sizeof(HwVolt);
        const esp_err_t err = nvs_get_blob(handle, "hwvolt", &hw, &size);
        ESP_LOGI(TAG, "nvs_get_blob hwvolt %d", err);
        if (err == ESP_OK) {
            ESP_LOGI(TAG, "get version from memory");
            ESP_LOGI(TAG, "hw.major = %d, hw.minor = %d", hw.major, hw.minor);
            nvs_close(handle);
            return hw;
        }
    }

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
    hw.major = esp_adc_cal_raw_to_voltage(hwMajor, &adc_chars) - 40;
    hw.minor = esp_adc_cal_raw_to_voltage(hwMinor, &adc_chars) - 40;
    ESP_LOGI(TAG, "get version from adc");
    ESP_LOGI(TAG, "hw.major = %d, hw.minor = %d", hw.major, hw.minor);

    if (openErr == ESP_OK) {
        const esp_err_t err = nvs_set_blob(handle, "hwvolt", &hw, sizeof(hw));
        ESP_LOGI(TAG, "nvs_set_blob hwvolt %d", err);
        nvs_commit(handle);
        nvs_close(handle);
        ESP_LOGI(TAG, "save version to memory");
    }

    return hw;
}

/**
 * @brief Read FW version data and return it
 *
 * @return Version::FwString structure with FW version
 */
Version::FwString EspVersion::getFwValue()
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
