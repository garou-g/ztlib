/*******************************************************************************
 * @file    serialdrv.hpp
 * @author  garou (xgaroux@gmail.com)
 * @brief   Header file of abstract serial peripheral driver.
 ******************************************************************************/

#pragma once

#if defined(FREERTOS_USED)
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#endif

#include "basedrv.h"

/**
 * @brief Abstract peripheral driver
 */
class SerialDrv : public BaseDrv {
public:
    /**
     * @brief Default constructor. Almost empty
     */
    SerialDrv();

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

    /**
     * @brief Sets address value for current operation
     *
     * @param addr new address value
     */
    void setAddr(int32_t addr);

private:
    int32_t reg_;
    int32_t addr_;
#if defined(FREERTOS_USED)
    SemaphoreHandle_t mutex_;
#endif
};

/***************************** END OF FILE ************************************/
