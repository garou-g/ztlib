/*******************************************************************************
 * @file    adc.h
 * @author  4lagest (zakirodion@gmail.com)
 * @brief   Header file of adc peripheral driver.
 ******************************************************************************/
#pragma once

#include "basedrv.h"

#include "etl/delegate.h"

enum class AdcMode {
    OneShoot,
    Continuous,
};

/// @brief ADC peripheral driver
class Adc : public BaseDrv {
public:
    /**
     * @brief Basic ADC commands
     */
    enum IoctlCmd {
        kSetResolution, // Sets ADC result resolution
        kSetSampleTime, // Sets ADC conversion sample time
        kSetMode,       // Sets ADC work mode
        kSetResultCb,   // Sets conversion finish callback
    };

    /**
     * @brief Starts ADC conversion
     */
    virtual void start() = 0;

    /**
     * @brief Stops ADC conversion
     */
    virtual void stop() = 0;

    /**
     * @brief Checks ADC conversion result ready
     *
     * @return true if result ready otherwise false
     */
    virtual bool isReady() = 0;

    /**
     * @brief Returns ADC conversion result
     *
     * @return uint16_t ADC result
     */
    virtual uint16_t value() = 0;

    /**
     * @brief Callback function for ADC conversion finish IRQ
     */
    using ResultDelegate = etl::delegate<void(Adc*, uint16_t)>;

    /**
     * @brief Sets the callback for ADC conversion finish IRQ
     *
     * @param resultCb callback delegate
     */
    void setResultDelegate(const ResultDelegate& resultCb)
    {
        resultCb_ = resultCb;
    }

protected:
    /**
     * @brief Returns reference on the callback for ADC conversion finish IRQ
     *
     * @return ResultDelegate& callback delegate
     */
    ResultDelegate& resultCb()
    {
        return resultCb_;
    }

private:
    ResultDelegate resultCb_;
};
