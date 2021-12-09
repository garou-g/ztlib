/*******************************************************************************
 * @file    system.hpp
 * @author  garou (xgaroux@gmail.com)
 * @brief   Header file of global system functions.
 ******************************************************************************/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __SYSTEM_H
#define __SYSTEM_H

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>

#include "version.hpp"

/* Exported constants --------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/

/**
 * @brief System class with system-wide functions
 *      such as HW and FW versions, boot status and etc.
 */
class System {
public:
    static void init();
    static System& getInstance();

    const Version::Hardware& hardwareVersion() const;
    const Version::Firmware& firmwareVersion() const;
    bool isFirstStart() const;
    uint16_t resetReason() const;
    uint16_t resetCounter() const;

protected:
    struct BootStatus {
        uint16_t resetReason;
        uint16_t resetCounter;
        uint16_t firstStart; // Need to be 0x55AA
    };

    /**
     * @brief Returns current boot status structure. Must be getter for
     *      noinit sleep retained memory.
     */
    virtual const BootStatus& bootStatus() const = 0;

    System(Version* ver);
    virtual ~System() {} // Empty virtual destructor for inheritance fix

private:
    static System* system;

    Version* version;
    Version::Hardware hardware;
    Version::Firmware firmware;
};

#endif /* __SYSTEM_H */

/***************************** END OF FILE ************************************/
