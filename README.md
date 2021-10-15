# fw-common-lib
Library with common modules for using in firmware's projects.

## Modules

### Time

Time management features. Can achieve system time and has static UTC time variable

### Module

Base module for different function modules. Has dispatcher method for time controlled calling of user code. Used with inheritance.

Module can separate own dispatcher into FreeRTOS task or used in main loop super cycle. For using FreeRTOS features use global define "FREERTOS_USED".

