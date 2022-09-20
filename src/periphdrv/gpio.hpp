/*******************************************************************************
 * @file    gpio.hpp
 * @author  garou (xgaroux@gmail.com)
 * @brief   Header file of abstract gpio driver.
 ******************************************************************************/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __GPIO_DRIVER_H
#define __GPIO_DRIVER_H

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>

/* Exported constants --------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/

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
     * @brief Default constructor. Almost empty
     */
    Gpio();

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
    bool isOpen() const;

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
    bool isInversed() const;

    /**
     * @brief Execute chosen command on driver
     *
     * @param cmd command to execute
     * @param pValue pointer for command data
     * @return true if command executed successfully otherwise false
     */
    virtual bool ioctl(uint32_t cmd, void* const pValue) = 0;

protected:
    /**
     * @brief Set opened state of driver from successor classes
     *
     * @param state new opened state
     */
    void setOpened(bool state);

    /**
     * @brief Set gpio inverse processing
     *
     * @param state new inverse state
     */
    void setInversed(bool state);

private:
    bool opened;
    bool inversed;
};

#endif /* __GPIO_DRIVER_H */

/***************************** END OF FILE ************************************/
