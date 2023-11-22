/*******************************************************************************
 * @file    system.cpp
 * @author  garou (xgaroux@gmail.com)
 * @brief   GD32 global system class functions.
 ******************************************************************************/

#include "system.h"
#include "version.h"
#include "gd32.h"
#include "attr.h"

#include <sys/time.h>

RETAIN_NOINIT_ATTR static uint32_t sysTime;

/**
 * @brief Constructor platform initialization
 *
 */
void System::platformInit()
{
    sysTime = 0;
    SysTick_Config(SystemCoreClock / 1000U); // 1 ms clock
    NVIC_SetPriority(SysTick_IRQn, 0x00U);

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

/**
 * @brief Acquiring system time and fill structure
 */
int _gettimeofday(struct timeval *tv, void *tzvp)
{
    const uint32_t t = sysTime;
    tv->tv_sec = t / 1000;
    tv->tv_usec = ( t % 1000 ) * 1000;
    return 0;
}

/**
 * @brief Systick IRQ handler increments system time
 */
void SysTick_Handler(void)
{
    ++sysTime;
}

/***************************** END OF FILE ************************************/
