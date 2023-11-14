/*******************************************************************************
 * @file    gd32i2c.hpp
 * @author  garou (xgaroux@gmail.com)
 * @brief   Header file of GD32 I2C bus driver.
 ******************************************************************************/

#pragma once

#include "i2c.hpp"
#include "gd32gpio.hpp"
#include "gd32.h"

class Gd32I2c final : public I2c {
public:
    Gd32I2c();

    struct Config {
        uint32_t i2c;
        Mode mode;
        Gd32Gpio::Config scl;
        Gd32Gpio::Config sda;
        int32_t speed;
    };

    bool open(const void* drvConfig) override;
    void close() override;

    bool ioctl(uint32_t cmd, void* pValue) override;

private:
    static constexpr uint32_t timeout = 5000;

    int32_t write_(const void* buf, uint32_t len) override;
    int32_t read_(void* buf, uint32_t len) override;

    bool waitForFlagSet(i2c_flag_enum flag) const;

    enum class State {
        Start,
        SendAddress,
        TransmitReg,
        TransmitData,
        Stop,
    };

    uint32_t i2c;
    int32_t addr;
};

/***************************** END OF FILE ************************************/
