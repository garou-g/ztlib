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
 */
void itoa(uint32_t value, char* buf)
{
    int i = 0;
    do {
        buf[i++] = value % 10 + '0';
    } while ((value /= 10) > 0);
    buf[i] = '\0';
    reverse(buf);
}

/**
 * @brief Converts float value to string
 *
 * @param value float value
 * @param buf chars buffer
 */
void ftoa(float value, char* buf)
{
    // Extract integer part
    int ipart = static_cast<int>(value);
    itoa(ipart, buf);
    int pos = 0;
    while (buf[pos++] != '\0');
    buf[pos-1] = '.';

    // Extract first fraction digit
    int fpart = static_cast<int>((value - static_cast<float>(ipart)) * 10);
    itoa(fpart, &buf[pos]);
}

/***************************** END OF FILE ************************************/
