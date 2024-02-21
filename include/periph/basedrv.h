/*******************************************************************************
 * @file    basedrv.hpp
 * @author  doctorbelka (avdbor.junior@mail.ru)
 * @brief   Header file of abstract peripheral driver.
 ******************************************************************************/

#pragma once

#include <stdint.h>

/// @brief Driver operations result enumeration
enum class DrvResult {
    Finished,
    InProgress,
    Error,
};

/**
 * @brief Abstract peripheral driver
 */
class BaseDrv {
public:
    /**
     * @brief Default constructor
     */
    BaseDrv() = default;

    /**
     * @brief Set current driver configuration for further usage
     *
     * @param drvConfig driver configuration
     * @return true if configuration accepted otherwise false
     */
    virtual bool setConfig(const void* drvConfig) = 0;

    /**
     * @brief Open driver with early saved configuration
     *
     * @return true if success opened otherwise false
     */
    virtual bool open() = 0;

    /**
     * @brief Open driver with selected config
     *
     * @param drvConfig driver configuration
     * @return true if success opened otherwise false
     */
    bool openWith(const void* drvConfig)
    {
        return setConfig(drvConfig) && open();
    }

    /**
     * @brief Check driver opened state
     *
     * @return true if opened otherwise false
     */
    bool isOpen() const
    {
        return opened_;
    }

    /**
     * @brief Close driver
     */
    virtual void close() = 0;

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
    void setOpened(bool state)
    {
        opened_ = state;
    }

private:
    bool opened_ = false;
};

/***************************** END OF FILE ************************************/
