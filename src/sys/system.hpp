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
    static void initInstance();
    static System& getInstance();

    const Version::Hardware& hardwareVersion() const;
    const Version::Firmware& firmwareVersion() const;
    const char* firmwareVersionString() const;

    enum ResetReason {
        kResetUnknown,
        kResetPowerOn,
        kResetSoftware,
        kResetPanic,
        kResetWatchdog,
        kResetSleep,
        kResetBrownout,
    };

    enum WakeupReason {
        kWakeupUnknown,
        kWakeupPin,
        kWakeupTimer,
    };

    virtual void restart() = 0;

    bool isFirstStart() const;
    uint16_t resetCounter() const;
    ResetReason resetReason() const;
    WakeupReason wakeupReason() const;

    virtual void goToSleep() const = 0;
    void setWakeupTime(uint32_t timeMs);
    uint32_t wakeupTime() const;
    void setWakeupPin(int32_t pin);
    int32_t wakeupPin() const;

protected:
    System(Version* ver);
    virtual ~System() {} // Empty virtual destructor for inheritance fix

    void setResetReason(ResetReason reset);
    void setWakeupReason(WakeupReason wakeup);

    struct SystemData {
        uint16_t firstStart; // Need to be 0x55AA
        uint16_t resetCounter;
        Version::Hardware hardware;
        Version::Firmware firmware;
    };

    static SystemData systemData;

private:
    static System* system;

    Version* version;
    char versionStr[Version::FW_SIZE];

    ResetReason _resetReason;
    WakeupReason _wakeupReason;
    uint32_t _wakeupTime;
    int32_t _wakeupPin;
};

#endif /* __SYSTEM_H */

/***************************** END OF FILE ************************************/
