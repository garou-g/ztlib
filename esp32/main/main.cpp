/*******************************************************************************
 * @file    main.cpp
 * @author  garou (xgaroux@gmail.com)
 * @brief   Main module.
 ******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "system.hpp"

/* Private typedef -----------------------------------------------------------*/
/* Private defines -----------------------------------------------------------*/
/* Private macros ------------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/

extern "C" void app_main(void)
{
    System::init();
    const System& system = System::getInstance();

    while (1) {
        vTaskDelay(100);
    }
}

/* Private functions ---------------------------------------------------------*/

/***************************** END OF FILE ************************************/
