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
            .clockdivision = TIMER_CKDIV_DIV1,
            .period = config_.period,
            .repetitioncounter = 0U
        };

        timer_init(config_.timer, &timerParams);

        // adding configurations for alternative modes
        switch (config_.mode)
        {
        case TimerMode::General:
            break;
        case TimerMode::Capture:
            timer_ic_parameter_struct timer_icinitpara;

            /* CH0 input capture configuration */
            timer_icinitpara.icpolarity = TIMER_IC_POLARITY_RISING;
            timer_icinitpara.icselection = TIMER_IC_SELECTION_DIRECTTI;
            timer_icinitpara.icprescaler = TIMER_IC_PSC_DIV1;
            timer_icinitpara.icfilter = 0x0;

            timer_input_capture_config(config_.timer, TIMER_CH_0, &timer_icinitpara);
            timer_auto_reload_shadow_enable(config_.timer);

            break;
        /* CH1 configuration in PWM mode1 */
        case TimerMode::Pwm:
            timer_oc_parameter_struct timer_ocintpara;
            timer_ocintpara.ocpolarity = TIMER_OC_POLARITY_HIGH;
            timer_ocintpara.outputstate = TIMER_CCX_ENABLE;
            timer_ocintpara.ocnpolarity = TIMER_OCN_POLARITY_HIGH;
            timer_ocintpara.outputnstate = TIMER_CCXN_DISABLE;
            timer_ocintpara.ocidlestate = TIMER_OC_IDLE_STATE_LOW;
            timer_ocintpara.ocnidlestate = TIMER_OCN_IDLE_STATE_LOW;

            timer_channel_output_config(config_.timer, TIMER_CH_1, &timer_ocintpara);
            timer_channel_output_pulse_value_config(config_.timer, TIMER_CH_1, config_.outputVal);
            timer_channel_output_mode_config(config_.timer, TIMER_CH_1, TIMER_OC_MODE_PWM0);
            timer_channel_output_shadow_config(config_.timer, TIMER_CH_1, TIMER_OC_SHADOW_DISABLE);

            /* auto-reload preload enable */
            timer_auto_reload_shadow_enable(config_.timer);

            break;
        }

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
    case kSetOutputVal:
        if (pValue != nullptr)
        {
            config_.outputVal = *(static_cast<uint32_t *>(pValue));
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
