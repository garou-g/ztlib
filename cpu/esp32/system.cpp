/*******************************************************************************
 * @file    system.cpp
 * @author  garou (xgaroux@gmail.com)
 * @brief   ESP global system class functions.
 ******************************************************************************/

#include "system.h"
#include "version.h"

#include "nvs_flash.h"
#include "esp_system.h"
#include "esp_sleep.h"
#include "hal/gpio_types.h"
#include "driver/gpio.h"
#include "driver/rtc_io.h"

/**
 * @brief Sets system frequency with chosen frequency in Hz and source
 *      (Not support for ESP32 now)
 *
 * @param freq frequency in Hz
 * @param freqSrc frequency source
 * @return true if frequency updated otherwise false
 */
bool System::setFrequency(uint32_t freq, SysFreqSrc freqSrc)
{
    return false;
}

/**
 * @brief Returns current system frequency
 *
 * @return uint32_t frequency in Hz
 */
uint32_t System::frequency() const
{
    return CONFIG_ESP32_DEFAULT_CPU_FREQ_MHZ * 1000000;
}

/**
 * @brief Constructor platform initialization
 *
 */
void System::platformInit()
{
    // Disable default pullup of IO5
    gpio_pullup_dis(GPIO_NUM_5);
    rtc_gpio_hold_dis(GPIO_NUM_4);

    // One time NVS flash init
    esp_err_t nvsErr = nvs_flash_init();
    if (nvsErr == ESP_ERR_NVS_NO_FREE_PAGES ||
        nvsErr == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        // NVS partition was truncated and needs to be erased
        // Retry nvs_flash_init
        nvs_flash_erase();
        nvs_flash_init();
    }

    // Get reset reason
    ResetReason reset;
    switch (esp_reset_reason()) {
    case ESP_RST_POWERON:
        reset = kResetPowerOn;
        break;
    case ESP_RST_SW:
        reset = kResetSoftware;
        break;
    case ESP_RST_PANIC:
        reset = kResetPanic;
        break;
    case ESP_RST_INT_WDT:
    case ESP_RST_TASK_WDT:
    case ESP_RST_WDT:
        reset = kResetWatchdog;
        break;
    case ESP_RST_DEEPSLEEP:
        reset = kResetSleep;
        break;
    case ESP_RST_BROWNOUT:
        reset = kResetBrownout;
        break;
    default:
        reset = kResetUnknown;
        break;
    }
    setResetReason(reset);

    // Get wakeup reason
    WakeupReason wakeup;
    if (reset == kResetSleep) {
        switch (esp_sleep_get_wakeup_cause()) {
        case ESP_SLEEP_WAKEUP_EXT0:
        case ESP_SLEEP_WAKEUP_EXT1:
        case ESP_SLEEP_WAKEUP_GPIO:
            wakeup = kWakeupPin;
            break;
        case ESP_SLEEP_WAKEUP_TIMER:
            wakeup = kWakeupTimer;
            break;
        default:
            wakeup = kWakeupUnknown;
            break;
        }
    } else {
        wakeup = kWakeupUnknown;
    }
    setWakeupReason(wakeup);
}

/**
 * @brief Restarts CPU
 */
void System::restart()
{
    esp_restart();
}

/**
 * @brief Switchs CPU to sleep mode with selected wakeup modes
 */
void System::goToSleep() const
{
    // Check wakeup time and enable wakeup event
    uint32_t time = wakeupTime();
    if (time != 0)
        esp_sleep_enable_timer_wakeup(time * 1000);

    // Check wakeup pin and enable wakeup event
    gpio_num_t pin = static_cast<gpio_num_t>(wakeupPin());
    if (pin != GPIO_NUM_NC) {
        uint64_t mask = 1 << pin;
        esp_sleep_enable_ext1_wakeup(mask, ESP_EXT1_WAKEUP_ALL_LOW);
    }

    esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_PERIPH, ESP_PD_OPTION_ON);

    // Enable gpio hold for leaving it state unchanged
    gpio_deep_sleep_hold_en();

    // Isolate pins from leakage
    rtc_gpio_isolate(GPIO_NUM_4);

    // Go to deep sleep. 150 uA without anything, 300 uA with sensors enabled
    esp_deep_sleep_start();
}

/***************************** END OF FILE ************************************/
