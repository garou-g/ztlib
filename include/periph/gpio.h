/*******************************************************************************
 * @file    gpio.h
 * @author  garou (xgaroux@gmail.com)
 * @brief   Header file of abstract gpio driver.
 ******************************************************************************/

#pragma once

#include <stdint.h>

/**
 * @brief Abstract gpio driver
 */
class Gpio {
public:
    struct Config {
        int32_t pin;
        bool inverse;
    };

    enum IoctlCmd {
    };

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
    virtual bool open(const void* drvConfig) = 0;

    /**
     * @brief Check driver opened state
     *
     * @return true if opened otherwise false
     */
    bool isOpen() const { return opened; }

    /**
     * @brief Close driver
     */
    virtual void close() = 0;

    /**
     * @brief Sets configured gpio to high level
     */
    virtual int32_t set() = 0;

    /**
     * @brief Sets configured gpio to low level
     */
    virtual int32_t reset() = 0;

    /**
     * @brief Check gpio inverse state processing
     *
     * @return true if gpio is inversed otherwise false
     */
    bool isInversed() const { return inversed; }

    /**
     * @brief Execute chosen command on driver
     *
     * @param cmd command to execute
     * @param pValue pointer for command data
     * @return true if command executed successfully otherwise false
     */
    virtual bool ioctl(uint32_t cmd, void* pValue) = 0;

protected:
    /**
     * @brief Set opened state of driver from successor classes
     *
     * @param state new opened state
     */
    void setOpened(bool state) { opened = state; }

    /**
     * @brief Set gpio inverse processing
     *
     * @param state new inverse state
     */
    void setInversed(bool state) { inversed = state; }

private:
    bool opened = false;
    bool inversed = false;
};

/***************************** END OF FILE ************************************/
