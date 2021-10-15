# fw-common-lib
Library with common modules for using in firmware's projects.

## Modules

### Time

Time management features. Can achieve system time and has static UTC time variable

### Module

Base module for different function modules. Has dispatcher method for time controlled calling of user code. Used with inheritance.

Module can separate own dispatcher into FreeRTOS task or used in main loop super cycle. For using FreeRTOS features use global define `FREERTOS_USED`.

### Version

Manages firmware and hardware versions by platform dependent realization in `hw` directory.

### System

Global system module that has option for determining first start of app, count of resets and reset reasons. Also it includes Version module inside. This module used through singleton pattern.

## Platform depended settings

- System and Version modules in ESP realization needs this modules: `driver esp_adc_cal app_update nvs_flash`

