# ******************************************************************************
# @file    CMakeLists.txt
# @author  garou (xgaroux@gmail.com)
# @brief   ZTLib CMake file for arm-none-eabi-gcc
# ******************************************************************************

cmake_minimum_required(VERSION 3.30)

project(ztlib)

set(MSG_PREFIX "ztlib |")

# Common includes --------------------------------------------------------------

list(APPEND ${PROJECT_NAME}_INCLUDES
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/include/sys
    ${CMAKE_CURRENT_SOURCE_DIR}/cpu
)

# Common sources ---------------------------------------------------------------

list(APPEND ${PROJECT_NAME}_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/periph/serialdrv.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/sys/module.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/sys/system.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/sys/timing.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/sys/version.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/crc.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/debug.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/utils.cpp
)

# Platform depending sources and includes --------------------------------------

if (NOT ZT_CPU_PLATFORM)
    set(ZT_CPU_PLATFORM None CACHE STRING "Default platform is none")
endif()

if(${ZT_CPU_PLATFORM} STREQUAL GD32)
    list(APPEND ${PROJECT_NAME}_DEFINES -DGD32_PLATFORM)

    # Add HAL components if defined
    if(CAN IN_LIST ZT_HAL)
        list(APPEND ${PROJECT_NAME}_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/cpu/gd32/can.cpp)
    endif()
    if(GPIO IN_LIST ZT_HAL)
        list(APPEND ${PROJECT_NAME}_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/cpu/gd32/gpio.cpp)
    endif()
    if(I2C IN_LIST ZT_HAL)
        list(APPEND ${PROJECT_NAME}_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/cpu/gd32/i2c.cpp)
    endif()
    if(SPI IN_LIST ZT_HAL)
        list(APPEND ${PROJECT_NAME}_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/cpu/gd32/spi.cpp)
    endif()
    if(TIMER IN_LIST ZT_HAL)
        list(APPEND ${PROJECT_NAME}_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/cpu/gd32/timer.cpp)
    endif()
    if(UART IN_LIST ZT_HAL)
        list(APPEND ${PROJECT_NAME}_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/cpu/gd32/uart.cpp)
    endif()

    # Add common platform components
    list(APPEND ${PROJECT_NAME}_SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/cpu/gd32/syscalls.c
        ${CMAKE_CURRENT_SOURCE_DIR}/cpu/gd32/system.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/cpu/gd32/version.cpp
    )

elseif(${ZT_CPU_PLATFORM} STREQUAL ESP32)
    list(APPEND ${PROJECT_NAME}_DEFINES -DESP32_PLATFORM)

    # Add HAL components if defined
    if(GPIO IN_LIST ZT_HAL)
        list(APPEND ${PROJECT_NAME}_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/cpu/esp32/gpio.cpp)
    endif()
    if(I2C IN_LIST ZT_HAL)
        list(APPEND ${PROJECT_NAME}_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/cpu/esp32/i2c.cpp)
    endif()
    if(UART IN_LIST ZT_HAL)
        list(APPEND ${PROJECT_NAME}_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/cpu/esp32/uart.cpp)
    endif()

    # Add common platform components
    list(APPEND ${PROJECT_NAME}_SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/cpu/esp32/system.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/cpu/esp32/version.cpp
    )
else()
    message(FATAL_ERROR "Unsupported CPU platform selected... (${ZT_CPU_PLATFORM})")
endif()

message(STATUS "${MSG_PREFIX} Platform is ${ZT_CPU_PLATFORM}")

# Setup library ----------------------------------------------------------------

add_library(${PROJECT_NAME} INTERFACE)
target_compile_definitions(${PROJECT_NAME} INTERFACE ${${PROJECT_NAME}_DEFINES})
target_include_directories(${PROJECT_NAME} INTERFACE ${${PROJECT_NAME}_INCLUDES})
target_sources(${PROJECT_NAME} INTERFACE ${${PROJECT_NAME}_SOURCES})
target_link_libraries(${PROJECT_NAME} INTERFACE)

# Append ETL library -----------------------------------------------------------

add_subdirectory(etl)
target_link_libraries(${PROJECT_NAME} INTERFACE etl::etl)
