/*******************************************************************************
 * @file    versoin.cpp
 * @author  garou (xgaroux@gmail.com)
 * @brief   Firmware and hardware version file.
 * @date    2021-06-29
 ******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "version.hpp"
#include <stdlib.h>

/* Private typedef -----------------------------------------------------------*/
/* Private defines -----------------------------------------------------------*/
/* Private macros ------------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/

/**
 * @brief Construct a new Version object
 */
Version::Version()
{
}

/**
 * @brief Reads hardware version from resistors network and fills structure
 *
 * @param hw reference for structure with HW versions
 */
void Version::getHardwareVersion(Hardware& hw)
{
    HwVolt hwVolt = getHwValue();
    hw = { 0, 0, 0 };
    if (hwVolt.major < 200) {
        hw.major1 = 0;
    } else if (hwVolt.major >= 200 && hwVolt.major < 400) {
        hw.major1 = 1;
    } else if (hwVolt.major >= 400 && hwVolt.major < 600) {
        hw.major1 = 2;
    } else if (hwVolt.major >= 600 && hwVolt.major < 800) {
        hw.major1 = 3;
    } else if (hwVolt.major >= 800 && hwVolt.major < 1000) {
        hw.major1 = 4;
    } else if (hwVolt.major >= 1000 && hwVolt.major < 1200) {
        hw.major1 = 5;
    } else if (hwVolt.major >= 1200 && hwVolt.major < 1400) {
        hw.major1 = 6;
    } else if (hwVolt.major >= 1400 && hwVolt.major < 1650) {
        hw.major1 = 7;
    } else if (hwVolt.major >= 1650 && hwVolt.major < 1850) {
        hw.major1 = 8;
    } else if (hwVolt.major >= 1850 && hwVolt.major < 2050) {
        hw.major1 = 9;
    } else if (hwVolt.major >= 2050 && hwVolt.major < 2250) {
        hw.major1 = 10;
    } else if (hwVolt.major >= 2250 && hwVolt.major < 2450) {
        hw.major1 = 11;
    } else if (hwVolt.major >= 2450 && hwVolt.major < 2650) {
        hw.major1 = 12;
    } else if (hwVolt.major >= 2650 && hwVolt.major < 2850) {
        hw.major1 = 13;
    } else if (hwVolt.major >= 2850 && hwVolt.major < 3050) {
        hw.major1 = 14;
    } else if (hwVolt.major >= 3050) {
        hw.major1 = 15;
    }

    // Parse minor board version
    if (hwVolt.minor < 200) {
        hw.minor1 = 0;
    } else if (hwVolt.minor >= 200 && hwVolt.minor < 400) {
        hw.minor1 = 1;
    } else if (hwVolt.minor >= 400 && hwVolt.minor < 600) {
        hw.minor1 = 2;
    } else if (hwVolt.minor >= 600 && hwVolt.minor < 800) {
        hw.minor1 = 3;
    } else if (hwVolt.minor >= 800 && hwVolt.minor < 1000) {
        hw.minor1 = 4;
    } else if (hwVolt.minor >= 1000 && hwVolt.minor < 1200) {
        hw.minor1 = 5;
    } else if (hwVolt.minor >= 1200 && hwVolt.minor < 1400) {
        hw.minor1 = 6;
    } else if (hwVolt.minor >= 1400 && hwVolt.minor < 1650) {
        hw.minor1 = 7;
    } else if (hwVolt.minor >= 1650 && hwVolt.minor < 1850) {
        hw.minor1 = 8;
    } else if (hwVolt.minor >= 1850 && hwVolt.minor < 2050) {
        hw.minor1 = 9;
    } else if (hwVolt.minor >= 2050 && hwVolt.minor < 2250) {
        hw.minor1 = 10;
    } else if (hwVolt.minor >= 2250 && hwVolt.minor < 2450) {
        hw.minor1 = 11;
    } else if (hwVolt.minor >= 2450 && hwVolt.minor < 2650) {
        hw.minor1 = 12;
    } else if (hwVolt.minor >= 2650 && hwVolt.minor < 2850) {
        hw.minor1 = 13;
    } else if (hwVolt.minor >= 2850 && hwVolt.minor < 3050) {
        hw.minor1 = 14;
    } else if (hwVolt.minor >= 3050) {
        hw.minor1 = 15;
    }
}

/**
 * @brief Reads firmware version and fills structure
 *      Using this format:
 *      [major1].[major2].[minor1].[minor2]-[commits ahead]-[commit hash]-[dirty]
 *      Example: 2.0.11.0-3-gebb41ea-dirty
 *
 * @param fw reference for structure with FW versions
 */
void Version::getFirmwareVersion(Firmware& fw)
{
    FwString fwString = getFwValue();
    char buf[FW_SIZE] = {0};
    int j = 0, prevJ = 0;

    // Parse version string
    fw = { 0, 0, 0, 0 };
    for (int i = 0; i < 4; i++) {
        int value = 0;
        for (; j < FW_SIZE; j++) {
            if (fwString.data[j] == '.' || fwString.data[j] == '-') {
                for (int k = 0; k < (j - prevJ); k++)
                    buf[k] = fwString.data[prevJ + k];
                buf[j - prevJ] = '\0';
                prevJ = ++j;
                value = atoi(buf);
                break;
            }
        }
        if (i == 0)
            fw.major1 = value;
        else if (i == 1)
            fw.major2 = value;
        else if (i == 2)
            fw.minor1 = value;
        else
            fw.minor2 = value;
    }
}

/* Private functions ---------------------------------------------------------*/

/***************************** END OF FILE ************************************/
