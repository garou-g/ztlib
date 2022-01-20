/*******************************************************************************
 * @file    espi2c.hpp
 * @author  garou (xgaroux@gmail.com)
 * @brief   Header file of ESP32 I2C bus driver.
 ******************************************************************************/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __ESP_I2C_DRIVER_H
#define __ESP_I2C_DRIVER_H

/* Includes ------------------------------------------------------------------*/
#include "driver/i2c.h"

#include "i2c.hpp"

/* Exported constants --------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/

class EspI2c final : public I2c {
public:
    EspI2c();

    bool open(const void* const drvConfig) override;
    void close() override;

    int32_t write(const void* const buf, uint32_t len) override;
    int32_t read(void* const buf, uint32_t len) override;

    bool ioctl(uint32_t cmd, void* const pValue) override;

private:
    static const uint32_t timeoutMs;

    i2c_port_t i2c;
    int32_t addr;
};

#endif /* __ESP_I2C_DRIVER_H */

/***************************** END OF FILE ************************************/
