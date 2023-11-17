/*******************************************************************************
 * @file    espsystem.hpp
 * @author  garou (xgaroux@gmail.com)
 * @brief   Header file of ESP32 global system functions.
 ******************************************************************************/

#pragma once

#include "system.hpp"

#include <stdint.h>

/**
 * @brief ESP system class with system-wide functions
 *      such as HW and FW versions, boot status and etc.
 */
class EspSystem final : public System {
public:
    EspSystem();

    void restart() override;

    void goToSleep() const override;
};

/***************************** END OF FILE ************************************/
