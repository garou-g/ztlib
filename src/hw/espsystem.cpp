/*******************************************************************************
 * @file    espsystem.cpp
 * @author  garou (xgaroux@gmail.com)
 * @brief   ESP global system class functions.
 ******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "esp_system.h"
#include "nvs_flash.h"
#include "hw/espversion.hpp"

#include "espsystem.hpp"
#include "espversion.hpp"

/* Private typedef -----------------------------------------------------------*/
/* Private defines -----------------------------------------------------------*/
/* Private macros ------------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

/**
 * @brief Global boot status structure for holding reset states
 */
RTC_NOINIT_ATTR System::BootStatus EspSystem::_bootStatus;

/* Exported functions --------------------------------------------------------*/

/**
 * @brief Construct a new EspSystem object
 */
EspSystem::EspSystem()
    : System(new EspVersion())
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

    // Reset states update
    _bootStatus.resetReason = (uint16_t)esp_reset_reason();
    if (_bootStatus.firstStart != 0x55AA) {
        _bootStatus.firstStart = 0x55AA;
        _bootStatus.resetCounter = 1;
    } else {
        _bootStatus.resetCounter++;
    }
}

/* Private functions ---------------------------------------------------------*/

/**
 * @brief Returns current boot status structure
 *
 * @return const BootStatus& current boot status
 */
const System::BootStatus& EspSystem::bootStatus() const
{
    return _bootStatus;
}

/***************************** END OF FILE ************************************/
