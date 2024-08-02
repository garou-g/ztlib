/*******************************************************************************
 * @file    i2c_types.h
 * @author  garou (xgaroux@gmail.com)
 * @brief   Header file of common I2C types.
 ******************************************************************************/

#pragma once

#include <cstdint>

#define CAN_DATA_MAX_SIZE 8U

struct CanMsg {
    uint32_t id;
    uint8_t data[CAN_DATA_MAX_SIZE];
    uint8_t size;
};

/***************************** END OF FILE ************************************/
