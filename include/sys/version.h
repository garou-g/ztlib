/*******************************************************************************
 * @file    version.h
 * @author  garou (xgaroux@gmail.com)
 * @brief   Header file of firmware and hardware versions.
 ******************************************************************************/

#pragma once

#include <stdint.h>

/**
 * @brief Class for firmware and hardware version reading. This class can
 *      be inherited for the actual hardware version determination. For this
 *      purposes it has two virtual methods getHwValue() and getFwValue()
 */
class Version {
public:
    static const int FW_SIZE = 32;

    // Hardware version structure with two major digits and two minor
    typedef struct {
        uint8_t major;
        uint8_t minor;
    } Hardware;

    // Firmware version structure with two major digits and two minor
    typedef struct {
        uint8_t major;
        uint8_t minor;
        uint8_t patch;
    } Firmware;

    Version();
    virtual ~Version() = default;

    void getHardwareVersion(Hardware& hw);
    void getFirmwareVersion(Firmware& fw, char* fwStr = nullptr);

private:
    struct FwString {
        char data[FW_SIZE];
    };

    Hardware getHwValue();
    FwString getFwValue();
};

/***************************** END OF FILE ************************************/
