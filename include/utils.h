/*******************************************************************************
 * @file    utils.h
 * @author  garou (xgaroux@gmail.com)
 * @brief   Header file of helper functions.
 ******************************************************************************/

#pragma once

#include <cstdint>

namespace utils {

void reverse(char* buf);
uint32_t itoa(int32_t value, char* buf);
uint32_t ftoa(float value, char* buf);
int32_t atoi(const char* buf);
float atof(const char* buf);

}; // namespace utils

/***************************** END OF FILE ************************************/
