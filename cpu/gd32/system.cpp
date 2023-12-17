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

bool System::setFrequency(uint32_t freq, SysFreqSrc freqSrc)
{
    bool res = true;
    uint32_t timeout = 0U;
    uint32_t flag = 0U;

#if defined(GD32F4XX_H)
    if (freqSrc == SysFreqSrc::External) {
        // Enable HXTAL and wait until it is stable
        RCU_CTL |= RCU_CTL_HXTALEN;
        do {
            timeout++;
            flag = RCU_CTL & RCU_CTL_HXTALSTB;
        } while (0 == flag && HXTAL_STARTUP_TIMEOUT != timeout);

        // Return on fail
        if (timeout == HXTAL_STARTUP_TIMEOUT) {
            return false;
        }
    } else {
        // Enable IRC16M and wait until it is stable
        RCU_CTL |= RCU_CTL_IRC16MEN;
        do {
            timeout++;
            flag = RCU_CTL & RCU_CTL_IRC16MSTB;
        } while (0 == flag && IRC16M_STARTUP_TIMEOUT != timeout);

        // Return on fail
        if (timeout == IRC16M_STARTUP_TIMEOUT) {
            return false;
        }
    }

    RCU_APB1EN |= RCU_APB1EN_PMUEN;
    PMU_CTL |= PMU_CTL_LDOVS;

    /* AHB = SYSCLK */
    RCU_CFG0 |= RCU_AHB_CKSYS_DIV1;
    /* APB2 = AHB/2 */
    RCU_CFG0 |= RCU_APB2_CKAHB_DIV2;
    /* APB1 = AHB/4 */
    RCU_CFG0 |= RCU_APB1_CKAHB_DIV4;

    if (freqSrc == SysFreqSrc::External) {
        switch (freq) {
        case 120000000:
            /* Configure the main PLL, PSC = 25, PLL_N = 240, PLL_P = 2, PLL_Q = 5 */
            RCU_PLL = (25U | (240U << 6U) | (((2U >> 1U) - 1U) << 16U) |
                (RCU_PLLSRC_HXTAL) | (5U << 24U));
            break;
        case 168000000:
            /* Configure the main PLL, PSC = 25, PLL_N = 336, PLL_P = 2, PLL_Q = 7 */
            RCU_PLL = (25U | (336U << 6U) | (((2U >> 1U) - 1U) << 16U) |
                (RCU_PLLSRC_HXTAL) | (7U << 24U));
            break;
        default: return false;
        }
    } else {
        switch (freq) {
        case 120000000:
            /* Configure the main PLL, PSC = 16, PLL_N = 240, PLL_P = 2, PLL_Q = 5 */
            RCU_PLL = (16U | (240U << 6U) | (((2U >> 1U) - 1U) << 16U) |
                (RCU_PLLSRC_IRC16M) | (5U << 24U));
            break;
        case 168000000:
            /* Configure the main PLL, PSC = 16, PLL_N = 336, PLL_P = 2, PLL_Q = 7 */
            RCU_PLL = (16U | (336U << 6U) | (((2U >> 1U) - 1U) << 16U) |
                (RCU_PLLSRC_IRC16M) | (7U << 24U));
            break;
        default: return false;
        }
    }

    /* enable PLL */
    RCU_CTL |= RCU_CTL_PLLEN;

    /* wait until PLL is stable */
    while(0U == (RCU_CTL & RCU_CTL_PLLSTB)){
    }

    /* Enable the high-drive to extend the clock frequency to 120 Mhz */
    PMU_CTL |= PMU_CTL_HDEN;
    while(0U == (PMU_CS & PMU_CS_HDRF)){
    }

    /* select the high-drive mode */
    PMU_CTL |= PMU_CTL_HDS;
    while(0U == (PMU_CS & PMU_CS_HDSRF)){
    }

    /* select PLL as system clock */
    RCU_CFG0 &= ~RCU_CFG0_SCS;
    RCU_CFG0 |= RCU_CKSYSSRC_PLLP;

    /* wait until PLL is selected as system clock */
    while(0U == (RCU_CFG0 & RCU_SCSS_PLLP)){
    }
#else
    #error("Not defined setFrequency for CPU version!")
#endif
    SystemCoreClock = freq;
    SysTick_Config(SystemCoreClock / 1000U);
    return true;
}

/**
 * @brief Constructor platform initialization
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
extern "C" int _gettimeofday(struct timeval *tv, void *tzvp)
{
    const uint32_t t = sysTime;
    tv->tv_sec = t / 1000;
    tv->tv_usec = ( t % 1000 ) * 1000;
    return 0;
}

/**
 * @brief Systick IRQ handler increments system time
 */
extern "C" void SysTick_Handler(void)
{
    ++sysTime;
}

/***************************** END OF FILE ************************************/
