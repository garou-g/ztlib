/*******************************************************************************
 * @file    timer.cpp
 * @author  doctorbelka (avdbor.junior@mail.ru)
 * @brief   GD32 Timer bus driver.
 ******************************************************************************/

#include "gd32/timer.h"

#include <cassert>

namespace gd32 {

static Timer* timerInstances[14] = { nullptr };

static bool checkConfig(const gd32::TimerConfig* config)
{
    assert(config != nullptr);
    return config->timer == TIMER0
        || config->timer == TIMER1
        || config->timer == TIMER2;
}

static void setHandler(uint32_t port, Timer* arg)
{
    switch (port) {
    case TIMER0: timerInstances[0] = arg; break;
    case TIMER1: timerInstances[1] = arg; break;
    case TIMER2: timerInstances[2] = arg; break;
    // case TIMER3: timerInstances[3] = arg; break;
    // case TIMER4: timerInstances[4] = arg; break;
    // case TIMER5: timerInstances[5] = arg; break;
    // case TIMER6: timerInstances[6] = arg; break;
    // case TIMER7: timerInstances[7] = arg; break;
    // case TIMER8: timerInstances[8] = arg; break;
    // case TIMER9: timerInstances[9] = arg; break;
    // case TIMER10: timerInstances[10] = arg; break;
    // case TIMER11: timerInstances[11] = arg; break;
    // case TIMER12: timerInstances[12] = arg; break;
    // case TIMER13: timerInstances[13] = arg; break;
    default: return;
    }
}

static void enableClock(uint32_t port)
{
    rcu_periph_enum portClk;
    switch (port) {
    case TIMER0: portClk = RCU_TIMER0; break;
    case TIMER1: portClk = RCU_TIMER1; break;
    case TIMER2: portClk = RCU_TIMER2; break;
    default: return;
    }
    rcu_periph_clock_enable(portClk);
}

static void setIrq(uint32_t port, bool enable)
{
    IRQn_Type irqType;
    switch (port) {
    case TIMER0: irqType = TIMER0_Channel_IRQn; break;
    case TIMER1: irqType = TIMER1_IRQn; break;
    case TIMER2: irqType = TIMER2_IRQn; break;
    default: return;
    }

    if (enable) {
        nvic_irq_enable(irqType, 0, 0);
    } else {
        nvic_irq_disable(irqType);
    }
}

bool Timer::setConfig(const void* drvConfig)
{
    if (isOpen())
        return false;

    const gd32::TimerConfig* config = static_cast<const gd32::TimerConfig*>(drvConfig);
    if (!checkConfig(config)) {
        return false;
    }
    config_ = *config;
    return true;
}

bool Timer::open()
{
    if (isOpen())
        return false;

    if (!checkConfig(&config_)) {
        return false;
    }

    initGpioPeriph(&config_.pin);

    enableClock(config_.timer);
    timer_deinit(config_.timer);
    timer_parameter_struct timerParams = {
        .prescaler = config_.prescaler,
        .alignedmode = TIMER_COUNTER_EDGE,
        .counterdirection = TIMER_COUNTER_UP,
        .clockdivision = TIMER_CKDIV_DIV1,
        .period = config_.period,
        .repetitioncounter = 0U,
    };
    timer_init(config_.timer, &timerParams);
    timer_auto_reload_shadow_enable(config_.timer);

    // Configurations for alternative modes
    switch (config_.mode) {
    case TimerMode::General:
        // Nothing more to configurate
        break;

    case TimerMode::Capture:
        timer_ic_parameter_struct timer_icinitpara;
        timer_icinitpara.icpolarity = TIMER_IC_POLARITY_RISING;
        timer_icinitpara.icselection = TIMER_IC_SELECTION_DIRECTTI;
        timer_icinitpara.icprescaler = TIMER_IC_PSC_DIV1;
        timer_icinitpara.icfilter = 0x0;

        switch (config_.channel) {
        case TIMER_CH_0: ch_flag_ = TIMER_INT_FLAG_CH0; break;
        case TIMER_CH_1: ch_flag_ = TIMER_INT_FLAG_CH1; break;
        case TIMER_CH_2: ch_flag_ = TIMER_INT_FLAG_CH2; break;
        case TIMER_CH_3: ch_flag_ = TIMER_INT_FLAG_CH3; break;
        }

        timer_input_capture_config(config_.timer, config_.channel, &timer_icinitpara);
        timer_interrupt_enable(config_.timer, ch_flag_);
        break;

    case TimerMode::Pwm:
        timer_oc_parameter_struct timer_ocintpara;
        timer_ocintpara.ocpolarity = TIMER_OC_POLARITY_HIGH;
        timer_ocintpara.outputstate = TIMER_CCX_ENABLE;
        timer_ocintpara.ocnpolarity = TIMER_OCN_POLARITY_HIGH;
        timer_ocintpara.outputnstate = TIMER_CCXN_DISABLE;
        timer_ocintpara.ocidlestate = TIMER_OC_IDLE_STATE_LOW;
        timer_ocintpara.ocnidlestate = TIMER_OCN_IDLE_STATE_LOW;

        timer_channel_output_config(config_.timer, config_.channel, &timer_ocintpara);
        timer_channel_output_pulse_value_config(config_.timer, config_.channel, config_.outputVal);
        timer_channel_output_mode_config(config_.timer, config_.channel, TIMER_OC_MODE_PWM0);
        timer_channel_output_shadow_config(config_.timer, config_.channel, TIMER_OC_SHADOW_DISABLE);
        break;
    }

    setHandler(config_.timer, this);
    setIrq(config_.timer, this);
    setOpened(true);
    return true;
}

void Timer::close()
{
    if (isOpen()) {
        timer_disable(config_.timer);
        timer_deinit(config_.timer);
        deinitGpioPeriph(&config_.pin);
        setIrq(config_.timer, false);
        setHandler(config_.timer, nullptr);
        setOpened(false);
        captured_ = 0;
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

uint32_t Timer::capturedFreq()
{
    if (config_.mode == TimerMode::Pwm)
        return 0;
    else
        return freq;
}

bool Timer::ioctl(uint32_t cmd, void* pValue)
{
    if (!isOpen())
        return false;

    switch (static_cast<IoctlCmd>(cmd)) {
    case kSetPrescale:
        if (pValue != nullptr) {
            config_.prescaler = *(static_cast<uint16_t*>(pValue));
            return true;
        }
        break;
    case kSetPeriod:
        if (pValue != nullptr) {
            config_.period = *(static_cast<uint32_t*>(pValue));
            return true;
        }
        break;
    case kSetOutputVal:
        if (pValue != nullptr) {
            config_.outputVal = *(static_cast<uint32_t*>(pValue));
            return true;
        }
        break;
    default:
        break;
    }

    return false;
}

void Timer::irqHandler(Timer* timer)
{
    // Check instance pointer
    if (!timer)
        return;

    if (SET == timer_interrupt_flag_get(timer->config_.timer, timer->ch_flag_)) {
        /* clear channel 0 interrupt bit */
        timer_interrupt_flag_clear(timer->config_.timer, timer->ch_flag_);

        if (0 == timer->ccnumber) {
            /* read channel 0 capture value */
            timer->readvalue1 = timer_channel_capture_value_register_read(timer->config_.timer, timer->config_.channel) + 1;
            timer->ccnumber = 1;
        } else if (1 == timer->ccnumber) {
            /* read channel 0 capture value */
            timer->readvalue2 = timer_channel_capture_value_register_read(timer->config_.timer, timer->config_.channel) + 1;

            if (timer->readvalue2 > timer->readvalue1) {
                timer->count = (timer->readvalue2 - timer->readvalue1);
            } else {
                timer->count = ((timer->config_.period - timer->readvalue1) + timer->readvalue2);
            }

            timer->freq = 8000000U / timer->count;
            timer->ccnumber = 0;
        }
    }
}

extern "C" void TIMER0_Channel_IRQHandler(void)
{
    Timer::irqHandler(timerInstances[0]);
}

extern "C" void TIMER1_IRQHandler(void)
{
    Timer::irqHandler(timerInstances[1]);
}

extern "C" void TIMER2_IRQHandler(void)
{
    Timer::irqHandler(timerInstances[2]);
}

}; // namespace gd32

/***************************** END OF FILE ************************************/
