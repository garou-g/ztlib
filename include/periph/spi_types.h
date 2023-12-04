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

enum class SpiPolarity {
    Low1Edge,
    High1Edge,
    Low2Edge,
    High2Edge,
};

enum class SpiFrame {
    Frame8Bit,
    Frame16Bit,
    Frame32Bit,
};

/***************************** END OF FILE ************************************/
