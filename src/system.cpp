/*******************************************************************************
 * @file    system.cpp
 * @author  garou (xgaroux@gmail.com)
 * @brief   Global system class functions.
 * @date    2021-05-10
 ******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "system.hpp"
#include "version.hpp"
#include <stdlib.h>

#if defined(ESP_PLATFORM)
#include "esp_system.h"
#include "nvs_flash.h"
#include "hw/espversion.hpp"
#else
#error("Not defined platform for version definition")
#endif

/* Private typedef -----------------------------------------------------------*/
/* Private defines -----------------------------------------------------------*/
/* Private macros ------------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

/**
 * @brief Global object pointer for singleton operations
 */
System* System::system = nullptr;

/**
 * @brief Global boot status structure for holding reset states
 */
__NOINIT_ATTR BootStatus System::bootStatus;

/* Exported functions --------------------------------------------------------*/

/**
 * @brief Init singleton instance. MUST BE called before any actions with module
 */
void System::init()
{
    if (system != nullptr)
        delete system;
    system = new System();
}

/**
 * @brief Returns global object instance
 *
 * @return System& reference to module object
 */
System& System::getInstance()
{
    return *system;
}

/**
 * @brief Returns reference to hardware version structure
 *
 * @return const HardwareVersion& hw version structure
 */
const Version::Hardware& System::hardwareVersion() const
{
    return hardware;
}

/**
 * @brief Returns reference to hardware version structure
 *
 * @return const FirmwareVersion& fw version structure
 */
const Version::Firmware& System::firmwareVersion() const
{
    return firmware;
}

/**
 * @brief Returns state of first boot of device
 *
 * @return true if device first time boots from power on, otherwise false
 */
bool System::isFirstStart() const
{
    return bootStatus.resetCounter == 1;
}

/**
 * @brief Returns code of previous reset reason
 *
 * @return uint16_t reset reason code
 */
uint16_t System::resetReason() const
{
    return bootStatus.resetReason;
}

/**
 * @brief Returns count of resets since device power on
 *
 * @return uint16_t reset count
 */
uint16_t System::resetCounter() const
{
    return bootStatus.resetCounter;
}

/* Private functions ---------------------------------------------------------*/

/**
 * @brief Construct a new System object
 */
System::System()
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

    // Fill hardware and firmware versions
#if defined(ESP_PLATFORM)
    version = new EspVersion();
#endif
    version->getHardwareVersion(hardware);
    version->getFirmwareVersion(firmware);
    delete version;

    // Reset states update
    bootStatus.resetReason = (uint16_t)esp_reset_reason();
    if (bootStatus.firstStart != 0x55AA) {
        bootStatus.firstStart = 0x55AA;
        bootStatus.resetCounter = 1;
    } else {
        bootStatus.resetCounter++;
    }
}

/***************************** END OF FILE ************************************/
