/*******************************************************************************
 * @file    crc.h
 * @author  garou (xgaroux@gmail.com)
 * @brief   Header file of CRC functions.
 ******************************************************************************/

#pragma once

#include <stdint.h>

uint8_t crc8(const uint8_t *buf, uint32_t len);
uint16_t crc16(const uint8_t *buf, uint32_t len);
uint32_t crc32(const uint8_t *buf, uint32_t len, const uint32_t* crcIn = nullptr);

/***************************** END OF FILE ************************************/
