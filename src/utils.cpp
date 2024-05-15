/*******************************************************************************
 * @file    utils.cpp
 * @author  garou (xgaroux@gmail.com)
 * @brief   Helper functions.
 ******************************************************************************/

#include "utils.h"

#include <cstring>

namespace utils {

/**
 * @brief Reverses chars buffer data
 *
 * @param buf chars buffer
 */
void reverse(char* buf)
{
    for (int i = 0, j = strlen(buf) - 1; i < j; i++, j--) {
        char c = buf[i];
        buf[i] = buf[j];
        buf[j] = c;
    }
}

/**
 * @brief Converts integer value to string in decimal format
 *
 * @param value integer value
 * @param buf chars buffer
 * @return uint32_t string size
 */
uint32_t itoa(uint32_t value, char* buf)
{
    int i = 0;
    do {
        buf[i++] = value % 10 + '0';
    } while ((value /= 10) > 0);
    buf[i] = '\0';
    reverse(buf);
    return i;
}

/**
 * @brief Converts float value to string
 *
 * @param value float value
 * @param buf chars buffer
 * @return uint32_t string size
 */
uint32_t ftoa(float value, char* buf)
{
    // Extract integer part
    int ipart = static_cast<int>(value);
    int pos = itoa(ipart, buf);
    buf[pos++] = '.';

    // Extract first fraction digit
    int fpart = static_cast<int>((value - static_cast<float>(ipart)) * 10);
    return itoa(fpart, &buf[pos]) + pos;
}

/**
 * @brief Converts string in decimal format to integer value
 *
 * @param buf chars buffer
 * @return uint32_t result integer
 */
int32_t atoi(const char* buf)
{
    // Skip whitespaces
    int32_t i = 0;
    while (buf[i] == ' ') {
        ++i;
    }

    // Check sign
    int32_t sign = 1;
    if (buf[i] == '-' || buf[i] == '+') {
        sign = 1 - 2 * (buf[i++] == '-');
    }

    // Calculate number
    int32_t base = 0;
    while (buf[i] >= '0' && buf[i] <= '9') {
        base = 10 * base + (buf[i++] - '0');
    }
    return base * sign;
}

}; // namespace utils

/***************************** END OF FILE ************************************/
