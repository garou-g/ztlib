/*******************************************************************************
 * @file    timer.h
 * @author  doctorbelka (avdbor.junior@mail.ru)
 * @brief   Header file of GD32 timer.
 ******************************************************************************/

#pragma once

#include "periph/timer.h"
#include "gd32/gd32_types.h"

namespace gd32 {

/// @brief Timer configuration data structure
struct TimerConfig {
    int32_t timer;
    uint16_t channel;
    uint16_t prescaler;
    uint32_t period;
    uint32_t pwmValue;
    TimerMode mode;
    GpioConfig pin;
};

/// @brief GD32 Timer peripheral driver
class Timer : public ::Timer {
public:
    Timer() = default;

    bool setConfig(const void* drvConfig) override;
    bool open() override;
    void close() override;

    bool ioctl(uint32_t cmd, void* pValue) override;

    void start() override;
    void stop() override;

    uint32_t captured() const override;
    uint32_t counter() const override;

    static void irqHandler(Timer* timer);

private:
    TimerConfig config_;
    uint32_t irqFlag_ = 0;
    uint32_t captured_ = 0;
};

}; // namespace gd32

/***************************** END OF FILE ************************************/
