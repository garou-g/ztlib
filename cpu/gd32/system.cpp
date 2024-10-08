/*******************************************************************************
 * @file    system.cpp
 * @author  garou (xgaroux@gmail.com)
 * @brief   GD32 global system class functions.
 ******************************************************************************/

#include "system.h"
#include "version.h"
#include "gd32/gd32.h"
#include "attr.h"

#include <sys/time.h>

RETAIN_NOINIT_ATTR static uint32_t sysTime;

/**
 * @brief Sets system frequency with chosen frequency in Hz and source
 *
 * @param freq frequency in Hz
 * @param freqSrc frequency source
 * @return true if frequency updated otherwise false
 */
bool System::setFrequency(uint32_t freq, SysFreqSrc freqSrc)
{
    bool res = true;
    uint32_t timeout = 0U;
    uint32_t flag = 0U;

#if defined(GD32F4XX_H)
    if (freqSrc == SysFreqSrc::ExternalBypass) {
        // Enable bypass mode
        RCU_CTL |= RCU_CTL_HXTALBPS;
    }

    if (freqSrc == SysFreqSrc::External
        || freqSrc == SysFreqSrc::ExternalBypass) {
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

    // AHB = SYSCLK
    RCU_CFG0 |= RCU_AHB_CKSYS_DIV1;
    // APB2 = AHB/2
    RCU_CFG0 |= RCU_APB2_CKAHB_DIV2;
    // APB1 = AHB/4
    RCU_CFG0 |= RCU_APB1_CKAHB_DIV4;

    if (freqSrc == SysFreqSrc::External
        || freqSrc == SysFreqSrc::ExternalBypass) {
        uint32_t psc;
        if (HXTAL_VALUE == 8000000)
            psc = 8U;
        else
            psc = 25U;

        switch (freq) {
        case 120000000:
            // Configure the main PLL, PSC = psc, PLL_N = 240, PLL_P = 2, PLL_Q = 5
            RCU_PLL = (psc | (240U << 6U) | (((2U >> 1U) - 1U) << 16U) |
                (RCU_PLLSRC_HXTAL) | (5U << 24U));
            break;
        case 168000000:
            // Configure the main PLL, PSC = psc, PLL_N = 336, PLL_P = 2, PLL_Q = 7
            RCU_PLL = (psc | (336U << 6U) | (((2U >> 1U) - 1U) << 16U) |
                (RCU_PLLSRC_HXTAL) | (7U << 24U));
            break;
        default: return false;
        }
    } else {
        switch (freq) {
        case 120000000:
            // Configure the main PLL, PSC = 16, PLL_N = 240, PLL_P = 2, PLL_Q = 5
            RCU_PLL = (16U | (240U << 6U) | (((2U >> 1U) - 1U) << 16U) |
                (RCU_PLLSRC_IRC16M) | (5U << 24U));
            break;
        case 168000000:
            // Configure the main PLL, PSC = 16, PLL_N = 336, PLL_P = 2, PLL_Q = 7
            RCU_PLL = (16U | (336U << 6U) | (((2U >> 1U) - 1U) << 16U) |
                (RCU_PLLSRC_IRC16M) | (7U << 24U));
            break;
        default: return false;
        }
    }

    // Enable PLL and wait until it is stable
    RCU_CTL |= RCU_CTL_PLLEN;
    while (0U == (RCU_CTL & RCU_CTL_PLLSTB)) {}

    // Enable the high-drive to extend the clock frequency to 120 Mhz
    PMU_CTL |= PMU_CTL_HDEN;
    while (0U == (PMU_CS & PMU_CS_HDRF)) {}

    // Select the high-drive mode
    PMU_CTL |= PMU_CTL_HDS;
    while (0U == (PMU_CS & PMU_CS_HDSRF)) {}

    // Select PLL as system clock
    RCU_CFG0 &= ~RCU_CFG0_SCS;
    RCU_CFG0 |= RCU_CKSYSSRC_PLLP;

    // Wait until PLL is selected as system clock
    while (0U == (RCU_CFG0 & RCU_SCSS_PLLP)) {}
#elif defined(GD32F30X_H)
    if (freqSrc == SysFreqSrc::ExternalBypass) {
        // Enable bypass mode
        RCU_CTL |= RCU_CTL_HXTALBPS;
    }

    if (freqSrc == SysFreqSrc::External
        || freqSrc == SysFreqSrc::ExternalBypass) {
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
        RCU_CTL |= RCU_CTL_IRC8MEN;
        do {
            timeout++;
            flag = RCU_CTL & RCU_CTL_IRC8MSTB;
        } while (0 == flag && IRC8M_STARTUP_TIMEOUT != timeout);

        // Return on fail
        if (timeout == IRC8M_STARTUP_TIMEOUT) {
            return false;
        }
    }

    RCU_APB1EN |= RCU_APB1EN_PMUEN;
    PMU_CTL |= PMU_CTL_LDOVS;

    // AHB = SYSCLK
    RCU_CFG0 |= RCU_AHB_CKSYS_DIV1;
    // APB2 = AHB/1
    RCU_CFG0 |= RCU_APB2_CKAHB_DIV1;
    // APB1 = AHB/2
    RCU_CFG0 |= RCU_APB1_CKAHB_DIV2;

    if (freqSrc == SysFreqSrc::External
        || freqSrc == SysFreqSrc::ExternalBypass) {
        switch (freq) {
        case 120000000:
#if (defined(GD32F30X_HD) || defined(GD32F30X_XD))
            // Select HXTAL/2 as clock source
            RCU_CFG0 &= ~(RCU_CFG0_PLLSEL | RCU_CFG0_PREDV0);
            RCU_CFG0 |= (RCU_PLLSRC_HXTAL_IRC48M | RCU_CFG0_PREDV0);

            // CK_PLL = (CK_HXTAL/2) * 30 = 120 MHz
            RCU_CFG0 &= ~(RCU_CFG0_PLLMF | RCU_CFG0_PLLMF_4 | RCU_CFG0_PLLMF_5);
            RCU_CFG0 |= RCU_PLL_MUL30;
#elif defined(GD32F30X_CL)
            // CK_PLL = (CK_PREDIV0) * 30 = 120 MHz
            RCU_CFG0 &= ~(RCU_CFG0_PLLMF | RCU_CFG0_PLLMF_4 | RCU_CFG0_PLLMF_5);
            RCU_CFG0 |= (RCU_PLLSRC_HXTAL_IRC48M | RCU_PLL_MUL30);

            // CK_PREDIV0 = (CK_HXTAL)/5 *8 /10 = 4 MHz
            RCU_CFG1 &= ~(RCU_CFG1_PLLPRESEL | RCU_CFG1_PREDV0SEL | RCU_CFG1_PLL1MF | RCU_CFG1_PREDV1 | RCU_CFG1_PREDV0);
            RCU_CFG1 |= (RCU_PLLPRESRC_HXTAL | RCU_PREDV0SRC_CKPLL1 | RCU_PLL1_MUL8 | RCU_PREDV1_DIV5 | RCU_PREDV0_DIV10);

            // Enable PLL1 and wait until it is stable
            RCU_CTL |= RCU_CTL_PLL1EN;
            while ((RCU_CTL & RCU_CTL_PLL1STB) == 0U) {}
#endif /* GD32F30X_HD and GD32F30X_XD */
            break;
        default: return false;
        }
    } else {
        switch (freq) {
        case 120000000:
            // CK_PLL = (CK_IRC8M/2) * 30 = 120 MHz
            RCU_CFG0 &= ~(RCU_CFG0_PLLMF | RCU_CFG0_PLLMF_4 | RCU_CFG0_PLLMF_5);
            RCU_CFG0 |= RCU_PLL_MUL30;
            break;
        default: return false;
        }
    }

    // Enable PLL and wait until it is stable
    RCU_CTL |= RCU_CTL_PLLEN;
    while (0U == (RCU_CTL & RCU_CTL_PLLSTB)) {}

    // Enable the high-drive to extend the clock frequency to 120 Mhz
    PMU_CTL |= PMU_CTL_HDEN;
    while (0U == (PMU_CS & PMU_CS_HDRF)) {}

    // Select the high-drive mode
    PMU_CTL |= PMU_CTL_HDS;
    while (0U == (PMU_CS & PMU_CS_HDSRF)) {}

    // Select PLL as system clock
    RCU_CFG0 &= ~RCU_CFG0_SCS;
    RCU_CFG0 |= RCU_CKSYSSRC_PLL;

    // Wait until PLL is selected as system clock
    while (0U == (RCU_CFG0 & RCU_SCSS_PLL)) {}
#elif defined(GD32F10X_MD) || defined(GD32F10X_HD) || \
    defined(GD32F10X_XD) || defined(GD32F10X_CL)
    if (freqSrc == SysFreqSrc::ExternalBypass) {
        // Enable bypass mode
        RCU_CTL |= RCU_CTL_HXTALBPS;
    }

    if (freqSrc == SysFreqSrc::External
        || freqSrc == SysFreqSrc::ExternalBypass) {
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
        // Enable IRC8M and wait until it is stable
        RCU_CTL |= RCU_CTL_IRC8MEN;
        do {
            timeout++;
            flag = RCU_CTL & RCU_CTL_IRC8MSTB;
        } while (0 == flag && IRC8M_STARTUP_TIMEOUT != timeout);

        // Return on fail
        if (timeout == IRC8M_STARTUP_TIMEOUT) {
            return false;
        }
    }

    // AHB = SYSCLK
    RCU_CFG0 |= RCU_AHB_CKSYS_DIV1;
    // APB2 = AHB/1
    RCU_CFG0 |= RCU_APB2_CKAHB_DIV1;
    // APB1 = AHB/2
    RCU_CFG0 |= RCU_APB1_CKAHB_DIV2;

    if (freqSrc == SysFreqSrc::External
        || freqSrc == SysFreqSrc::ExternalBypass) {
        switch (freq) {
        case 108000000:
#if (defined(GD32F10X_MD) || defined(GD32F10X_HD) || defined(GD32F10X_XD))
            // Select HXTAL/2 as clock source
            RCU_CFG0 &= ~(RCU_CFG0_PLLSEL | RCU_CFG0_PREDV0);
            RCU_CFG0 |= (RCU_PLLSRC_HXTAL | RCU_CFG0_PREDV0);

            // CK_PLL = (CK_HXTAL/2) * 27 = 108 MHz
            RCU_CFG0 &= ~(RCU_CFG0_PLLMF | RCU_CFG0_PLLMF_4);
            RCU_CFG0 |= RCU_PLL_MUL27;

#elif defined(GD32F10X_CL)
            // CK_PLL = (CK_PREDIV0) * 27 = 108 MHz
            RCU_CFG0 &= ~(RCU_CFG0_PLLMF | RCU_CFG0_PLLMF_4);
            RCU_CFG0 |= (RCU_PLLSRC_HXTAL | RCU_PLL_MUL27);

            // CK_PREDIV0 = (CK_HXTAL)/5 *8 /10 = 4 MHz
            RCU_CFG1 &= ~(RCU_CFG1_PREDV0SEL | RCU_CFG1_PLL1MF | RCU_CFG1_PREDV1 | RCU_CFG1_PREDV0);
            RCU_CFG1 |= (RCU_PREDV0SRC_CKPLL1 | RCU_PLL1_MUL8 | RCU_PREDV1_DIV5 | RCU_PREDV0_DIV10);

            // Enable PLL1 and wait until it is stable
            RCU_CTL |= RCU_CTL_PLL1EN;
            while (0U == (RCU_CTL & RCU_CTL_PLL1STB)) {}
#endif /* GD32F10X_MD and GD32F10X_HD and GD32F10X_XD */
            break;
        default: return false;
        }
    } else {
        switch (freq) {
        case 108000000:
            // CK_PLL = (CK_IRC8M/2) * 27 = 108 MHz
            RCU_CFG0 &= ~(RCU_CFG0_PLLMF | RCU_CFG0_PLLMF_4);
            RCU_CFG0 |= RCU_PLL_MUL27;
            break;
        default: return false;
        }
    }

    // Enable PLL and wait until it is stable
    RCU_CTL |= RCU_CTL_PLLEN;
    while (0U == (RCU_CTL & RCU_CTL_PLLSTB)) {}

    // Select PLL as system clock
    RCU_CFG0 &= ~RCU_CFG0_SCS;
    RCU_CFG0 |= RCU_CKSYSSRC_PLL;

    // Wait until PLL is selected as system clock
    while (RCU_SCSS_PLL != (RCU_CFG0 & RCU_CFG0_SCSS)) {}
#else
    #error("Not defined setFrequency for CPU version!")
#endif
    SystemCoreClock = freq;
    SysTick_Config(SystemCoreClock / 1000U);
    return true;
}

/**
 * @brief Returns current system frequency
 *
 * @return uint32_t frequency in Hz
 */
uint32_t System::frequency() const
{
    return SystemCoreClock;
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
    // Disable low power debug for prevent hanging after debug session
    dbg_low_power_disable(DBG_LOW_POWER_SLEEP | DBG_LOW_POWER_DEEPSLEEP |
        DBG_LOW_POWER_STANDBY);

    pmu_wakeup_pin_enable();
    pmu_to_standbymode();
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
