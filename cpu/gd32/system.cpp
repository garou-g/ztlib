/*******************************************************************************
 * @file    system.cpp
 * @author  garou (xgaroux@gmail.com)
 * @brief   GD32 global system class functions.
 ******************************************************************************/

#include "system.h"
#include "version.h"
#include "gd32.h"

/**
 * @brief Constructor platform initialization
 *
 */
void System::platformInit()
{
    // Get reset reason
    ResetReason reset;
    if (SET == rcu_flag_get(RCU_FLAG_EPRST)) {
        reset = kResetPowerOn;
    } else if (SET == rcu_flag_get(RCU_FLAG_PORRST)) {
        reset = kResetPowerOn;
    } else if (SET == rcu_flag_get(RCU_FLAG_SWRST)) {
        reset = kResetSoftware;
    } else if (SET == rcu_flag_get(RCU_FLAG_FWDGTRST)) {
        reset = kResetWatchdog;
    } else if (SET == rcu_flag_get(RCU_FLAG_WWDGTRST)) {
        reset = kResetWatchdog;
    } else if (SET == rcu_flag_get(RCU_FLAG_LPRST)) {
        reset = kResetSleep;
    } else {
        reset = kResetUnknown;
    }
    setResetReason(reset);
    rcu_all_reset_flag_clear();

    // Get wakeup reason
    setWakeupReason(kWakeupUnknown);
}

/**
 * @brief Restarts CPU
 */
void System::restart()
{
    NVIC_SystemReset();
}

/**
 * @brief Switchs CPU to sleep mode with selected wakeup modes
 */
void System::goToSleep() const
{
    // Not realizied
}

/***************************** END OF FILE ************************************/
