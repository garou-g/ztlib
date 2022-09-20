/*******************************************************************************
 * @file    espgpio.hpp
 * @author  garou (xgaroux@gmail.com)
 * @brief   Header file of ESP32 GPIO driver.
 ******************************************************************************/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __ESP_GPIO_DRIVER_H
#define __ESP_GPIO_DRIVER_H

/* Includes ------------------------------------------------------------------*/
#include "driver/gpio.h"

#include "gpio.hpp"

/* Exported constants --------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/

class EspGpio final : public Gpio {
public:
    EspGpio();

    bool open(const void* drvConfig) override;
    void close() override;

    int32_t set() override;
    int32_t reset() override;

    bool ioctl(uint32_t cmd, void* pValue) override;

private:
    gpio_num_t pin;
};

#endif /* __ESP_GPIO_DRIVER_H */

/***************************** END OF FILE ************************************/
