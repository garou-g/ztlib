/*******************************************************************************
 * @file    serialdrv.cpp
 * @author  garou (xgaroux@gmail.com)
 * @brief   Abstract serial peripheral driver.
 ******************************************************************************/

#include "periph/serialdrv.h"

SerialDrv::SerialDrv()
    : reg_(-1)
    , addr_(-1)
{
#if defined(FREERTOS_USED)
    mutex_ = xSemaphoreCreateBinary();
    xSemaphoreGive(mutex_);
#endif
}

int32_t SerialDrv::write(const void* buf, uint32_t len)
{
#if defined(FREERTOS_USED)
    xSemaphoreTake(mutex_, portMAX_DELAY);
#endif
    int32_t res = write_(buf, len);
#if defined(FREERTOS_USED)
    xSemaphoreGive(mutex_);
#endif
    return res;
}

int32_t SerialDrv::write(uint8_t reg, const void* buf, uint32_t len)
{
#if defined(FREERTOS_USED)
    xSemaphoreTake(mutex_, portMAX_DELAY);
#endif
    reg_ = reg;
    int32_t res = write_(buf, len);
    reg_ = -1; // Mark that no actual register value after writing
#if defined(FREERTOS_USED)
    xSemaphoreGive(mutex_);
#endif
    return res;
}

int32_t SerialDrv::write(uint32_t addr, uint8_t reg, const void* buf, uint32_t len)
{
#if defined(FREERTOS_USED)
    xSemaphoreTake(mutex_, portMAX_DELAY);
#endif
    addr_ = addr;
    reg_ = reg;
    int32_t res = write_(buf, len);
    addr_ = -1;
    reg_ = -1; // Mark that no actual register value after writing
#if defined(FREERTOS_USED)
    xSemaphoreGive(mutex_);
#endif
    return res;
}

int32_t SerialDrv::read(void* buf, uint32_t len)
{
    #if defined(FREERTOS_USED)
    xSemaphoreTake(mutex_, portMAX_DELAY);
#endif
    int32_t res = read_(buf, len);
#if defined(FREERTOS_USED)
    xSemaphoreGive(mutex_);
#endif
    return res;
}

int32_t SerialDrv::read(uint8_t reg, void* buf, uint32_t len)
{
#if defined(FREERTOS_USED)
    xSemaphoreTake(mutex_, portMAX_DELAY);
#endif
    reg_ = reg;
    int32_t res = read_(buf, len);
    reg_ = -1; // Mark that no actual register value after writing
#if defined(FREERTOS_USED)
    xSemaphoreGive(mutex_);
#endif
    return res;
}

int32_t SerialDrv::read(uint32_t addr, uint8_t reg, void* buf, uint32_t len)
{
#if defined(FREERTOS_USED)
    xSemaphoreTake(mutex_, portMAX_DELAY);
#endif
    addr_ = addr;
    reg_ = reg;
    int32_t res = read_(buf, len);
    addr_ = -1;
    reg_ = -1; // Mark that no actual register value after writing
#if defined(FREERTOS_USED)
    xSemaphoreGive(mutex_);
#endif
    return res;
}

int32_t SerialDrv::getReg() const
{
    return reg_;
}

int32_t SerialDrv::getAddr() const
{
    return addr_;
}

void SerialDrv::setAddr(int32_t addr)
{
    addr_ = addr;
}

/***************************** END OF FILE ************************************/
