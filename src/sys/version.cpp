/*******************************************************************************
 * @file    versoin.cpp
 * @author  garou (xgaroux@gmail.com)
 * @brief   Firmware and hardware version file.
 ******************************************************************************/

#include "version.h"

#include <stdlib.h>
#include <string.h>

/**
 * @brief Construct a new Version object
 */
Version::Version()
{
}

/**
 * @brief Reads hardware version and fills structure
 *
 * @param hw reference for structure with HW versions
 */
void Version::getHardwareVersion(Hardware& hw)
{
    hw = getHwValue();
}

/**
 * @brief Reads firmware version and fills structure
 *      Using format from https://semver.org:
 *      [major].[minor].[patch]
 *      Source data can contain additional information from git:
 *      [major].[minor].[patch (commits ahead)]-[commit hash]-[dirty]
 *
 *      Example: 0.3-2-g58a0619-dirty
 *
 * @param fw reference for structure with FW versions
 * @param fwStr reference for string with FW versions, can be null
 */
void Version::getFirmwareVersion(Firmware& fw, char* fwStr)
{
    FwString fwString = getFwValue();
    fw = { 0, 0, 0 };

    int j = 0;
    for (int i = 0; i < 3; ++i) {
        int value = 0;
        int k = 0;
        char buf[FW_SIZE] = { 0 };
        for (; j < FW_SIZE; ++j) {
            // Replace - symbol
            if (fwString.data[j] == '-') {
                fwString.data[j] = '.';
            }

            // Delimiter symbols is . and zero
            if (fwString.data[j] == '.'
                || fwString.data[j] == '\0') {
                buf[k] = '\0';
                value = atoi(buf);
                ++j;
                break;
            // If next symbol is not digit - fw string is ended
            } else if (fwString.data[j] < '0'
                || fwString.data[j] > '9') {
                // Change symbol to zero char
                fwString.data[j] = '0';
                value = 0;
                ++j;
                break;
            // Take digit to buffer
            } else {
                buf[k++] = fwString.data[j];
            }
        }
        if (i == 0)
            fw.major = value;
        else if (i == 1)
            fw.minor = value;
        else
            fw.patch = value;
    }

    // Copy actual part of version in result buffer if present
    if (fwStr != nullptr) {
        memcpy(fwStr, fwString.data, j-1);
        fwStr[j-1] = '\0';
    }
}

/***************************** END OF FILE ************************************/
