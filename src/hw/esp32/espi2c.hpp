/*******************************************************************************
 * @file    espi2c.hpp
 * @author  garou (xgaroux@gmail.com)
 * @brief   Header file of ESP32 I2C bus driver.
 ******************************************************************************/

#pragma once


#include "i2c.hpp"

#include "driver/i2c.h"

class EspI2c final : public I2c {
public:
    EspI2c();

    bool open(const void* drvConfig) override;
    void close() override;

    bool ioctl(uint32_t cmd, void* pValue) override;

private:
    static const uint32_t timeoutMs;

    int32_t write_(const void* buf, uint32_t len) override;
    int32_t read_(void* buf, uint32_t len) override;

    i2c_port_t i2c;
    int32_t addr;
    int32_t scl;
    int32_t sda;
};

/***************************** END OF FILE ************************************/
