/*******************************************************************************
 * @file    system.cpp
 * @author  garou (xgaroux@gmail.com)
 * @brief   Global system class functions.
 ******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include <assert.h>

#include "system.hpp"
#include "systemproxy.hpp"

/* Private typedef -----------------------------------------------------------*/
/* Private defines -----------------------------------------------------------*/
/* Private macros ------------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

/**
 * @brief Global object pointer for singleton operations
 */
System* System::system = nullptr;

/* Exported functions --------------------------------------------------------*/

/**
 * @brief Init singleton instance. MUST BE called before any actions with module
 */
void System::init()
{
    if (system != nullptr)
        delete system;
    system = SystemProxy::initSystemByPlatform();
}

/**
 * @brief Returns global object instance
 *
 * @return System& reference to module object
 */
System& System::getInstance()
{
    assert(system != nullptr);
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
    return bootStatus().resetCounter == 1;
}

/**
 * @brief Returns code of previous reset reason
 *
 * @return uint16_t reset reason code
 */
uint16_t System::resetReason() const
{
    return bootStatus().resetReason;
}

/**
 * @brief Returns count of resets since device power on
 *
 * @return uint16_t reset count
 */
uint16_t System::resetCounter() const
{
    return bootStatus().resetCounter;
}

/* Private functions ---------------------------------------------------------*/

/**
 * @brief Construct a new System object
 *
 * @param ver version instance
 */
System::System(Version* ver)
    : version(ver)
{
    assert(version != nullptr);
    version->getHardwareVersion(hardware);
    version->getFirmwareVersion(firmware);
}

/***************************** END OF FILE ************************************/
