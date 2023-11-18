/*******************************************************************************
 * @file    gpio.h
 * @author  garou (xgaroux@gmail.com)
 * @brief   Header file of abstract gpio driver.
 ******************************************************************************/

#pragma once

#include <stdint.h>

/// @brief Basic gpio driver config structure
struct GpioConfig {
    uint32_t pin;
    bool inverse;
};

/// @brief Abstract gpio driver
class Gpio {
public:
    /**
     * @brief Default constructor
     */
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
    bool isOpen() const { return port_ != -1; }

    /**
     * @brief Close driver
     */
    void close();

    /**
     * @brief Sets configured gpio to high level
     */
    void set();

    /**
     * @brief Sets configured gpio to low level
     */
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
    /// @brief Gpio port number
    int32_t port_ = -1;

    /// @brief Gpio port pin number
    int32_t pin_ = -1;

    /// @brief Flag of gpio inversion parameter
    bool inverse_ = false;
};

/***************************** END OF FILE ************************************/
