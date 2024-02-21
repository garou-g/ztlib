/*******************************************************************************
 * @file    timer.cpp
 * @author  doctorbelka (avdbor.junior@mail.ru)
 * @brief   GD32 Timer bus driver.
 ******************************************************************************/

#include "gd32/timer.h"

#include <cassert>

namespace gd32 {

    static bool checkConfig(const gd32::TimerConfig *config)
    {
        assert(config != nullptr);
        return config->timer == TIMER0 || config->timer == TIMER1 || config->timer == TIMER2;
    }

    bool Timer::setConfig(const void *drvConfig)
    {
        if (isOpen())
            return false;

        const gd32::TimerConfig *config = static_cast<const gd32::TimerConfig *>(drvConfig);
        if (!checkConfig(config))
        {
            return false;
        }
        config_ = *config;
        return true;
    }

    bool Timer::open()
    {
        if (isOpen())
            return false;

        if (!checkConfig(&config_))
        {
            return false;
        }

        rcu_periph_enum timerClock;
        if (config_.timer == TIMER0)
        {
            timerClock = RCU_TIMER0;
        }
        else if (config_.timer == TIMER1)
        {
            timerClock = RCU_TIMER1;
        }
        else
        {
            timerClock = RCU_TIMER2;
        }
        rcu_periph_clock_enable(timerClock);

        initGpioPeriph(&config_.pin);

        timer_deinit(config_.timer);
        timer_parameter_struct timerParams = {
            .prescaler = config_.prescaler,
            .alignedmode = TIMER_COUNTER_EDGE,
            .counterdirection = TIMER_COUNTER_UP,
            .period = config_.period,
            .clockdivision = TIMER_CKDIV_DIV1,
            .repetitioncounter = 0U};
        // switch (config_.mode) {
        // case gd32::TimerConfig::General: break;
        // case gd32::TimerConfig::Capture: ;
        // case gd32::TimerConfig::Pwm: ;
        // }

        timer_init(config_.timer, &timerParams);
        setOpened(true);
        return true;
    }

void Timer::close()
{
    if (isOpen())
    {
        timer_disable(config_.timer);
        deinitGpioPeriph(&config_.pin);
        captured_ = 0;
        timer_deinit(config_.timer);
        setOpened(false);
    }
}

void Timer::start()
{
    if (!isOpen())
        return;
    timer_enable(config_.timer);
}
void Timer::stop()
{
    if (!isOpen())
        return;
    timer_disable(config_.timer);
}

uint32_t Timer::captured()
{
    return captured_;
}

bool Timer::ioctl(uint32_t cmd, void *pValue)
{
    if (!isOpen())
        return false;

    switch (static_cast<IoctlCmd>(cmd))
    {
    case kSetPrescale:
        if (pValue != nullptr)
        {
            config_.prescaler = *(static_cast<uint16_t *>(pValue));
            return true;
        }
        break;
    case kSetPeriod:
        if (pValue != nullptr)
        {
            config_.period = *(static_cast<uint32_t *>(pValue));
            return true;
        }
        break;
    default:
        break;
    }

    return false;
}

}; // namespace gd32

/***************************** END OF FILE ************************************/
