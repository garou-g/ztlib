/*******************************************************************************
 * @file    utils.cpp
 * @author  garou (xgaroux@gmail.com)
 * @brief   Helper functions.
 ******************************************************************************/

#include "utils.h"

#include <cstring>

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

/***************************** END OF FILE ************************************/
