/*******************************************************************************
 * @file    timer.cpp
 * @author  doctorbelka (avdbor.junior@mail.ru)
 * @brief   GD32 timer driver.
 ******************************************************************************/

#include "gd32/timer.h"

#include <cassert>

namespace gd32 {

/// @brief Timer instances pointers table for IRQ usage
static Timer* timerInstances[14] = { nullptr };

/**
 * @brief Check timer configuration validity
 *
 * @param config timer configuration
 * @return true if configuration is valid otherwise false
 */
static bool checkConfig(const gd32::TimerConfig* config)
{
    assert(config != nullptr);
#if defined(GD32F30X_HD) || defined(GD32F30X_XD) || defined(GD32F30X_CL)
        return config->timer == TIMER0 || config->timer == TIMER1
        || config->timer == TIMER2 || config->timer == TIMER3
        || config->timer == TIMER4 || config->timer == TIMER5
        || config->timer == TIMER6 || config->timer == TIMER7
#endif
#if defined(GD32F30X_XD) || defined(GD32F30X_CL)
        || config->timer == TIMER8 || config->timer == TIMER9
        || config->timer == TIMER10 || config->timer == TIMER11
        || config->timer == TIMER12 || config->timer == TIMER13
#endif
    ;
}

/**
 * @brief Sets timer pointer to instances table according to chosen timer
 *
 * @param port timer periph
 * @param arg Timer instance pointer
 */
static void setHandler(uint32_t port, Timer* arg)
{
    switch (port) {
#if defined(GD32F30X_HD) || defined(GD32F30X_XD) || defined(GD32F30X_CL)
    case TIMER0: timerInstances[0] = arg; break;
    case TIMER1: timerInstances[1] = arg; break;
    case TIMER2: timerInstances[2] = arg; break;
    case TIMER3: timerInstances[3] = arg; break;
    case TIMER4: timerInstances[4] = arg; break;
    case TIMER5: timerInstances[5] = arg; break;
    case TIMER6: timerInstances[6] = arg; break;
    case TIMER7: timerInstances[7] = arg; break;
#endif
#if defined(GD32F30X_XD) || defined(GD32F30X_CL)
    case TIMER8: timerInstances[8] = arg; break;
    case TIMER9: timerInstances[9] = arg; break;
    case TIMER10: timerInstances[10] = arg; break;
    case TIMER11: timerInstances[11] = arg; break;
    case TIMER12: timerInstances[12] = arg; break;
    case TIMER13: timerInstances[13] = arg; break;
#endif
    default: return;
    }
}

/**
 * @brief Enables timer clock according to chosen timer
 *
 * @param timer timer periph
 */
static void enableClock(uint32_t timer)
{
    rcu_periph_enum clk;
    switch (timer) {
#if defined(GD32F30X_HD) || defined(GD32F30X_XD) || defined(GD32F30X_CL)
    case TIMER0: clk = RCU_TIMER0; break;
    case TIMER1: clk = RCU_TIMER1; break;
    case TIMER2: clk = RCU_TIMER2; break;
    case TIMER3: clk = RCU_TIMER3; break;
    case TIMER4: clk = RCU_TIMER4; break;
    case TIMER5: clk = RCU_TIMER5; break;
    case TIMER6: clk = RCU_TIMER6; break;
    case TIMER7: clk = RCU_TIMER7; break;
#endif
#if defined(GD32F30X_XD) || defined(GD32F30X_CL)
    case TIMER8: clk = RCU_TIMER8; break;
    case TIMER9: clk = RCU_TIMER9; break;
    case TIMER10: clk = RCU_TIMER10; break;
    case TIMER11: clk = RCU_TIMER11; break;
    case TIMER12: clk = RCU_TIMER12; break;
    case TIMER13: clk = RCU_TIMER13; break;
#endif
    default: return;
    }
    rcu_periph_clock_enable(clk);
}

/**
 * @brief Sets IRQ new state according to chosen timer
 *
 * @param config timer configuration
 * @param state new IRQ state
 */
static void setIrq(const gd32::TimerConfig& config, bool state)
{
    IRQn_Type irqType;
    switch (config.timer) {
#if defined(GD32F30X_HD) || defined(GD32F30X_XD) || defined(GD32F30X_CL)
    case TIMER0:
        if (config.mode == TimerMode::General) {
            irqType = TIMER0_UP_IRQn;
        } else if (config.mode == TimerMode::Capture) {
            irqType = TIMER0_Channel_IRQn;
        } else {
            return;
        }
        break;
    case TIMER1: irqType = TIMER1_IRQn; break;
    case TIMER2: irqType = TIMER2_IRQn; break;
    case TIMER3: irqType = TIMER3_IRQn; break;
    case TIMER4: irqType = TIMER4_IRQn; break;
    case TIMER5: irqType = TIMER5_IRQn; break;
    case TIMER6: irqType = TIMER6_IRQn; break;
    case TIMER7:
        if (config.mode == TimerMode::General) {
            irqType = TIMER7_UP_IRQn;
        } else if (config.mode == TimerMode::Capture) {
            irqType = TIMER7_Channel_IRQn;
        } else {
            return;
        }
        break;
#endif
#if defined(GD32F30X_XD) || defined(GD32F30X_CL)
    case TIMER8: irqType = TIMER0_BRK_TIMER8_IRQn; break;
    case TIMER9: irqType = TIMER0_UP_TIMER9_IRQn; break;
    case TIMER10: irqType = TIMER0_TRG_CMT_TIMER10_IRQn; break;
    case TIMER11: irqType = TIMER7_BRK_TIMER11_IRQn; break;
    case TIMER12: irqType = TIMER7_UP_TIMER12_IRQn; break;
    case TIMER13: irqType = TIMER7_TRG_CMT_TIMER13_IRQn; break;
#endif
    default: return;
    }

    if (state) {
        nvic_irq_enable(irqType, 0, 0); // Timer IRQ has highest priority
    } else {
        nvic_irq_disable(irqType);
    }
}

/**
 * @brief Set current driver configuration for further usage
 *
 * @param drvConfig driver configuration
 * @return true if configuration accepted otherwise false
 */
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

/**
 * @brief Open driver with early saved configuration
 *
 * @return true if success opened otherwise false
 */
bool Timer::open()
{
    if (isOpen() || !checkConfig(&config_))
        return false;

    initGpioPeriph(&config_.pin);

    enableClock(config_.timer);
    timer_deinit(config_.timer);
    timer_parameter_struct timerParams = {
        .prescaler = config_.prescaler,
        .alignedmode = TIMER_COUNTER_EDGE,
        .counterdirection = TIMER_COUNTER_UP,
        .clockdivision = TIMER_CKDIV_DIV1,
        .period = config_.period,
        .repetitioncounter = 0,
    };
    timer_init(config_.timer, &timerParams);
    timer_auto_reload_shadow_enable(config_.timer);

    switch (config_.mode) {
    case TimerMode::General:
        irqFlag_ = TIMER_INT_FLAG_UP;
        timer_interrupt_enable(config_.timer, irqFlag_);
        break;

    case TimerMode::Capture:
        timer_ic_parameter_struct timer_icinitpara;
        timer_icinitpara.icpolarity = TIMER_IC_POLARITY_RISING;
        timer_icinitpara.icselection = TIMER_IC_SELECTION_DIRECTTI;
        timer_icinitpara.icprescaler = TIMER_IC_PSC_DIV1;
        timer_icinitpara.icfilter = 0;
        timer_input_capture_config(config_.timer, config_.channel, &timer_icinitpara);

        switch (config_.channel) {
        default:
        case TIMER_CH_0: irqFlag_ = TIMER_INT_FLAG_CH0; break;
        case TIMER_CH_1: irqFlag_ = TIMER_INT_FLAG_CH1; break;
        case TIMER_CH_2: irqFlag_ = TIMER_INT_FLAG_CH2; break;
        case TIMER_CH_3: irqFlag_ = TIMER_INT_FLAG_CH3; break;
        }
        timer_interrupt_enable(config_.timer, irqFlag_);
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
        timer_channel_output_pulse_value_config(config_.timer, config_.channel, config_.pwmValue);
        timer_channel_output_mode_config(config_.timer, config_.channel, TIMER_OC_MODE_PWM0);
        timer_channel_output_shadow_config(config_.timer, config_.channel, TIMER_OC_SHADOW_DISABLE);
        break;
    }

    setHandler(config_.timer, this);
    setIrq(config_, true);
    setOpened(true);
    return true;
}

/**
 * @brief Close driver
 */
void Timer::close()
{
    if (isOpen()) {
        timer_disable(config_.timer);
        timer_deinit(config_.timer);
        deinitGpioPeriph(&config_.pin);
        setIrq(config_, false);
        setHandler(config_.timer, nullptr);
        setOpened(false);
        captured_ = 0;
    }
}

/**
 * @brief Starts timer counting
 */
void Timer::start()
{
    if (!isOpen())
        return;
    timer_enable(config_.timer);
}

/**
 * @brief Stops timer counting
 */
void Timer::stop()
{
    if (!isOpen())
        return;
    timer_disable(config_.timer);
}

/**
 * @brief Returns last captured value
 *
 * @return uint32_t captured value
 */
uint32_t Timer::captured() const
{
    return captured_;
}

/**
 * @brief Returns current timer counter value
 *
 * @return uint32_t timer counter
 */
uint32_t Timer::counter() const
{
    return timer_counter_read(config_.timer);
}

/**
 * @brief Execute chosen command on driver
 *
 * @param cmd command to execute
 * @param pValue pointer for command data
 * @return true if command executed successfully otherwise false
 */
bool Timer::ioctl(uint32_t cmd, void* pValue)
{
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
    case kSetPwmVal:
        if (pValue != nullptr) {
            config_.pwmValue = *(static_cast<uint32_t*>(pValue));
            return true;
        }
        break;
    default:
        break;
    }

    return false;
}

/**
 * @brief Timer IRQ processing handler
 *
 * @param timer instance pointer
 */
void Timer::irqHandler(Timer* timer)
{
    // Check instance pointer
    if (!timer)
        return;

    // Check chosen interrupt flag
    if (timer_interrupt_flag_get(timer->config_.timer, timer->irqFlag_) == SET) {
        timer_interrupt_flag_clear(timer->config_.timer, timer->irqFlag_);

        if (timer->config_.mode == TimerMode::General) {
            timer->generalCb().call_if();
        } else if (timer->config_.mode == TimerMode::Capture) {
            timer->captured_ = timer_channel_capture_value_register_read(
                timer->config_.timer, timer->config_.channel) + 1;
            timer->captureCb().call_if(timer->captured_);
        }
    }
}

#if defined(GD32F30X_HD) || defined(GD32F30X_XD) || defined(GD32F30X_CL)
extern "C" void TIMER0_UP_IRQHandler()
{
    Timer::irqHandler(timerInstances[0]);
}

extern "C" void TIMER0_Channel_IRQHandler()
{
    Timer::irqHandler(timerInstances[0]);
}

extern "C" void TIMER1_IRQHandler()
{
    Timer::irqHandler(timerInstances[1]);
}

extern "C" void TIMER2_IRQHandler()
{
    Timer::irqHandler(timerInstances[2]);
}

extern "C" void TIMER3_IRQHandler()
{
    Timer::irqHandler(timerInstances[3]);
}

extern "C" void TIMER4_IRQHandler()
{
    Timer::irqHandler(timerInstances[4]);
}

extern "C" void TIMER5_IRQHandler()
{
    Timer::irqHandler(timerInstances[5]);
}

extern "C" void TIMER6_IRQHandler()
{
    Timer::irqHandler(timerInstances[6]);
}

extern "C" void TIMER7_UP_IRQHandler()
{
    Timer::irqHandler(timerInstances[7]);
}

extern "C" void TIMER7_Channel_IRQHandler()
{
    Timer::irqHandler(timerInstances[7]);
}

#endif
#if defined(GD32F30X_XD) || defined(GD32F30X_CL)
extern "C" void TIMER0_BRK_TIMER8_IRQHandler()
{
    Timer::irqHandler(timerInstances[8]);
}

extern "C" void TIMER0_UP_TIMER9_IRQHandler()
{
    Timer::irqHandler(timerInstances[9]);
}

extern "C" void TIMER0_TRG_CMT_TIMER10_IRQHandler()
{
    Timer::irqHandler(timerInstances[10]);
}

extern "C" void TIMER7_BRK_TIMER11_IRQHandler()
{
    Timer::irqHandler(timerInstances[11]);
}

extern "C" void TIMER7_UP_TIMER12_IRQHandler()
{
    Timer::irqHandler(timerInstances[12]);
}

extern "C" void TIMER7_TRG_CMT_TIMER13_IRQHandler()
{
    Timer::irqHandler(timerInstances[13]);
}
#endif

}; // namespace gd32

/***************************** END OF FILE ************************************/
