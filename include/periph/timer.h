/*******************************************************************************
 * @file    timer.h
 * @author  doctorbelka (avdbor.junior@mail.ru)
 * @brief   Header file of timer.
 ******************************************************************************/

#pragma once

#include "basedrv.h"

#include "etl/delegate.h"

/// @brief Timer modes enumeration
enum class TimerMode {
    General,
    Capture,
    GeneralAndCapture,
    Pwm,
};

/// @brief Timer peripheral driver
class Timer : public BaseDrv {
public:
    /**
     * @brief Basic timer commands
     */
    enum IoctlCmd {
        kSetPrescale,   // Sets timer prescale
        kSetPeriod,     // Sets repetition value
        kSetPwmVal,     // Sets output value for PWM mode
        kSetGeneralCb,  // Sets timer basic callback
        kSetCaptureCb,  // Sets timer capture callback
    };

    /**
     * @brief Starts timer counting
     */
    virtual void start() = 0;

    /**
     * @brief Stops timer counting
     */
    virtual void stop() = 0;

    /**
     * @brief Returns last captured value
     *
     * @return uint32_t captured value
     */
    virtual uint32_t captured() const = 0;

    /**
     * @brief Sets current timer counter value
     *
     * @param counter timer counter
     */
    virtual void setCounter(uint32_t counter) = 0;

    /**
     * @brief Returns current timer counter value
     *
     * @return uint32_t timer counter
     */
    virtual uint32_t counter() const = 0;

    /**
     * @brief Callback function for timer counting IRQ
     */
    using GeneralDelegate = etl::delegate<void(void)>;

    /**
     * @brief Callback function for timer capture IRQ
     */
    using CaptureDelegate = etl::delegate<void(uint32_t)>;

    /**
     * @brief Sets the callback for counting IRQ
     *
     * @param generalCb callback delegate
     */
    void setGeneralDelegate(const GeneralDelegate& generalCb)
    {
        generalCb_ = generalCb;
    }

    /**
     * @brief Sets the callback for capture IRQ
     *
     * @param captureCb callback delegate
     */
    void setCaptureDelegate(const CaptureDelegate& captureCb)
    {
        captureCb_ = captureCb;
    }

protected:
    /**
     * @brief Returns reference on the callback for counting IRQ
     *
     * @return GeneralDelegate& callback delegate
     */
    GeneralDelegate& generalCb()
    {
        return generalCb_;
    }

    /**
     * @brief Returns reference on the callback for capture IRQ
     *
     * @return CaptureDelegate& callback delegate
     */
    CaptureDelegate& captureCb()
    {
        return captureCb_;
    }

private:
    GeneralDelegate generalCb_;
    CaptureDelegate captureCb_;
};

/***************************** END OF FILE ************************************/
