/*******************************************************************************
 * @file    system.h
 * @author  garou (xgaroux@gmail.com)
 * @brief   Header file of global system functions.
 ******************************************************************************/

#pragma once

#include "version.h"

#include <stdint.h>

enum class SysFreqSrc {
    External,
    Internal,
};

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

    bool setFrequency(uint32_t freq, SysFreqSrc freqSrc = SysFreqSrc::External);

    void restart();
    void goToSleep() const;

    bool isFirstStart() const;
    uint16_t resetCounter() const;
    ResetReason resetReason() const;
    WakeupReason wakeupReason() const;

    void setWakeupTime(uint32_t timeMs);
    uint32_t wakeupTime() const;
    void setWakeupPin(int32_t pin);
    int32_t wakeupPin() const;

protected:
    System(Version* ver);
    virtual ~System() = default;
    void platformInit();

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

    ResetReason resetReason_;
    WakeupReason wakeupReason_;
    uint32_t wakeupTime_;
    int32_t wakeupPin_;
};

/***************************** END OF FILE ************************************/
