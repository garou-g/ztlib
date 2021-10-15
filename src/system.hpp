/*******************************************************************************
 * @file    system.hpp
 * @author  garou (xgaroux@gmail.com)
 * @brief   Header file of global system functions.
 ******************************************************************************/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __SYSTEM_H
#define __SYSTEM_H

/* Includes ------------------------------------------------------------------*/
#include "version.hpp"
#include <stdint.h>

/* Exported constants --------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/

/**
 * @brief Boot status structure handles reset reason and counter and etc.
 */
typedef struct {
    uint16_t resetReason;
    uint16_t resetCounter;
    uint16_t firstStart; // Need to be 0x55AA
} BootStatus;

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

private:
    static System* system;
    static BootStatus bootStatus;

    System();

    Version* version;
    Version::Hardware hardware;
    Version::Firmware firmware;
};

#endif /* __SYSTEM_H */

/***************************** END OF FILE ************************************/
