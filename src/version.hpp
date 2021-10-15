/*******************************************************************************
 * @file    version.hpp
 * @author  garou (xgaroux@gmail.com)
 * @brief   Header file of firmware and hardware versions.
 ******************************************************************************/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __VERSION_H
#define __VERSION_H

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>

/* Exported constants --------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/

/**
 * @brief Class for firmware and hardware version reading. This class can
 *      be inherited for the actual hardware version determination. For this
 *      purposes it has two virtual methods getHwValue() and getFwValue()
 */
class Version {
public:
    // Hardware version structure with two major digits and one minor
    typedef struct {
        uint8_t major1;
        uint8_t major2;
        uint8_t minor1;
    } Hardware;

    // Firmware version structure with two major digits and two minor
    typedef struct {
        uint8_t major1;
        uint8_t major2;
        uint8_t minor1;
        uint8_t minor2;
    } Firmware;

    Version();
    virtual ~Version() {} // Empty virtual destructor for inheritance fix

    void getHardwareVersion(Hardware& hw);
    void getFirmwareVersion(Firmware& fw);

protected:
    struct HwVolt {
        int major;
        int minor;
    };

    static const int FW_SIZE = 32;
    struct FwString {
        char data[FW_SIZE];
    };

    virtual HwVolt getHwValue() { return HwVolt(); };
    virtual FwString getFwValue() { return FwString(); };
};

#endif /* __VERSION_H */

/***************************** END OF FILE ************************************/