/*******************************************************************************
 * @file    gd32i2c.hpp
 * @author  garou (xgaroux@gmail.com)
 * @brief   Header file of GD32 I2C bus driver.
 ******************************************************************************/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __GD32_I2C_DRIVER_H
#define __GD32_I2C_DRIVER_H

/* Includes ------------------------------------------------------------------*/
#include "gd32f30x.h"

#include "i2c.hpp"
#include "gd32gpio.hpp"

/* Class definition ----------------------------------------------------------*/

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

    int32_t write(const void* buf, uint32_t len) override;
    int32_t read(void* buf, uint32_t len) override;

    bool ioctl(uint32_t cmd, void* pValue) override;

private:
    static constexpr uint32_t timeout = 5000;

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

#endif /* __ESP_I2C_DRIVER_H */

/***************************** END OF FILE ************************************/
