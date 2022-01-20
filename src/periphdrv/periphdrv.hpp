/*******************************************************************************
 * @file    periphdrv.hpp
 * @author  garou (xgaroux@gmail.com)
 * @brief   Header file of abstract peripheral driver.
 ******************************************************************************/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __PERIPHDRV_DRIVER_H
#define __PERIPHDRV_DRIVER_H

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>

/* Exported constants --------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/

/**
 * @brief Abstract peripheral driver
 */
class PeriphDrv {
public:
    /**
     * @brief Default constructor. Almost empty
     */
    PeriphDrv();

    /**
     * @brief Open driver with selected config
     *
     * @param drvConfig driver configuration
     * @return true if success opened otherwise false
     */
    virtual bool open(const void* const drvConfig) = 0;

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
     * @brief Write data to driver
     *
     * @param buf data buffer
     * @param len length of data buffer
     * @return int32_t actually written data length, -1 on error
     */
    virtual int32_t write(const void* const buf, uint32_t len) = 0;

    /**
     * @brief Write data to driver with register access
     *
     * @param reg register to write data
     * @param buf data buffer
     * @param len length of data buffer
     * @return int32_t actually written data length, -1 on error
     */
    int32_t write(uint8_t reg, const void* const buf, uint32_t len);

    /**
     * @brief Receive data from driver
     *
     * @param buf data buffer
     * @param len length of data to read
     * @return int32_t actually readed data length, -1 on error
     */
    virtual int32_t read(void* const buf, uint32_t len) = 0;

    /**
     * @brief Receive data from driver with register access
     *
     * @param reg register to write data
     * @param buf data buffer
     * @param len length of data to read
     * @return int32_t actually readed data length, -1 on error
     */
    int32_t read(uint8_t reg, void* const buf, uint32_t len);

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
     * @brief Get register value for current operation
     *
     * @return uint8_t register value
     */
    int32_t getReg() const;

private:
    bool opened;
    int32_t reg;
};

#endif /* __PERIPHDRV_DRIVER_H */

/***************************** END OF FILE ************************************/
