/*******************************************************************************
 * @file    gpio.h
 * @author  garou (xgaroux@gmail.com)
 * @brief   Header file of abstract gpio driver.
 ******************************************************************************/

#pragma once

#include <stdint.h>

/// @brief Abstract gpio driver
class Gpio {
public:
    /// @brief Default constructor
    Gpio() = default;

    /**
     * @brief Open driver with selected config
     *
     * @param drvConfig driver configuration
     * @return true if success opened otherwise false
     */
    bool open(const void* drvConfig);

    /**
     * @brief Check driver opened state
     *
     * @return true if opened otherwise false
     */
    bool isOpen() const { return config_ != nullptr; }

    /// @brief Close driver
    void close();

    /// @brief Sets configured gpio to high level
    void set();

    /// @brief Sets configured gpio to low level
    void reset();

    /**
     * @brief Execute chosen command on driver
     *
     * @param cmd command to execute
     * @param pValue pointer for command data
     * @return true if command executed successfully otherwise false
     */
    bool ioctl(uint32_t cmd, void* pValue);

private:
    void *config_ = nullptr;
};

/***************************** END OF FILE ************************************/
