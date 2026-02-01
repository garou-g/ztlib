/*******************************************************************************
 * @file    spi_types.h
 * @author  garou (xgaroux@gmail.com)
 * @brief   Header file of common SPI types.
 ******************************************************************************/

#pragma once

enum class SpiMode {
    Master,
    Slave,
};

enum class SpiPolarity : uint8_t {
    Low1Edge = 0,
    High1Edge = 1,
    Low2Edge = 2,
    High2Edge = 3,
};

enum class SpiFrame : uint8_t {
    Frame8Bit = 8,
    Frame16Bit = 16,
    Frame32Bit = 32,
};

/***************************** END OF FILE ************************************/
