/*******************************************************************************
 * @file    gd32gpio.hpp
 * @author  garou (xgaroux@gmail.com)
 * @brief   Header file of GD32 GPIO driver.
 ******************************************************************************/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __GD32_GPIO_DRIVER_H
#define __GD32_GPIO_DRIVER_H

/* Includes ------------------------------------------------------------------*/
#include "gd32f30x.h"

#include "gpio.hpp"

/* Class definition ----------------------------------------------------------*/

class Gd32Gpio final : public Gpio {
public:
    Gd32Gpio();

    struct Config {
        rcu_periph_enum clock;
        uint32_t port;
        uint32_t pin;
        uint32_t mode;
        bool inverse;
    };

    bool open(const void* drvConfig) override;
    void close() override;

    int32_t set() override;
    int32_t reset() override;

    bool ioctl(uint32_t cmd, void* pValue) override;

private:
    bool isPortExist(uint32_t port) const;

    uint32_t port;
    int32_t pin;
};

#endif /* __GD32_GPIO_DRIVER_H */

/***************************** END OF FILE ************************************/
