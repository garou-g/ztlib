/*******************************************************************************
 * @file    espuart.hpp
 * @author  garou (xgaroux@gmail.com)
 * @brief   Header file of ESP32 UART driver.
 ******************************************************************************/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __ESP_UART_DRIVER_H
#define __ESP_UART_DRIVER_H

/* Includes ------------------------------------------------------------------*/
#include "driver/uart.h"

#include "uart.hpp"

/* Exported constants --------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/

class EspUart final : public Uart {
public:
    EspUart();

    bool open(const void* drvConfig) override;
    void close() override;

    int32_t write(const void* buf, uint32_t len) override;
    int32_t read(void* buf, uint32_t len) override;

    bool ioctl(uint32_t cmd, void* pValue) override;

private:
    int32_t uart;
    int32_t tx;
    int32_t rx;
};

#endif /* __ESP_UART_DRIVER_H */

/***************************** END OF FILE ************************************/
