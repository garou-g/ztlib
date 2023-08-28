/*******************************************************************************
 * @file    periphdrv.hpp
 * @author  garou (xgaroux@gmail.com)
 * @brief   Header file of abstract peripheral driver.
 ******************************************************************************/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __PERIPH_DRIVER_H
#define __PERIPH_DRIVER_H

/* Includes ------------------------------------------------------------------*/
#if defined(FREERTOS_USED)
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#endif

#include <stdint.h>

/* Class definition ----------------------------------------------------------*/

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
     * @brief Write data to driver
     *
     * @param buf data buffer
     * @param len length of data buffer
     * @return int32_t actually written data length, -1 on error
     */
    int32_t write(const void* buf, uint32_t len);

    /**
     * @brief Write data to driver with register access
     *
     * @param reg register to write data
     * @param buf data buffer
     * @param len length of data buffer
     * @return int32_t actually written data length, -1 on error
     */
    int32_t write(uint8_t reg, const void* buf, uint32_t len);

    /**
     * @brief Write data to driver with register access and address
     *
     * @param addr address of device
     * @param reg register to write data
     * @param buf data buffer
     * @param len length of data buffer
     * @return int32_t actually written data length, -1 on error
     */
    int32_t write(uint32_t addr, uint8_t reg, const void* buf, uint32_t len);

    /**
     * @brief Receive data from driver
     *
     * @param buf data buffer
     * @param len length of data to read
     * @return int32_t actually readed data length, -1 on error
     */
    int32_t read(void* buf, uint32_t len);

    /**
     * @brief Receive data from driver with register access
     *
     * @param reg register to write data
     * @param buf data buffer
     * @param len length of data to read
     * @return int32_t actually readed data length, -1 on error
     */
    int32_t read(uint8_t reg, void* buf, uint32_t len);

    /**
     * @brief Receive data from driver with register access andd address
     *
     * @param addr address of device
     * @param reg register to write data
     * @param buf data buffer
     * @param len length of data to read
     * @return int32_t actually readed data length, -1 on error
     */
    int32_t read(uint32_t addr, uint8_t reg, void* buf, uint32_t len);

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
     * @brief Abstract write data to driver
     *
     * @param buf data buffer
     * @param len length of data buffer
     * @return int32_t actually written data length, -1 on error
     */
    virtual int32_t write_(const void* buf, uint32_t len) = 0;

    /**
     * @brief Abstract receive data from driver
     *
     * @param buf data buffer
     * @param len length of data to read
     * @return int32_t actually readed data length, -1 on error
     */
    virtual int32_t read_(void* buf, uint32_t len) = 0;

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

    /**
     * @brief Get address value for current operation
     *
     * @return int32_t address value
     */
    int32_t getAddr() const;

private:
    bool opened_;
    int32_t reg_;
    int32_t addr_;
#if defined(FREERTOS_USED)
    SemaphoreHandle_t mutex_;
#endif
};

#endif /* __PERIPH_DRIVER_H */

/***************************** END OF FILE ************************************/
