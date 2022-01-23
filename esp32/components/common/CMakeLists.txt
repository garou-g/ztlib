set(src_path "../../../src")
set(periph_path "${src_path}/periphdrv")

idf_component_register(SRCS "${src_path}/base/module.cpp" "${src_path}/base/time.cpp"
                        "${src_path}/sys/version.cpp" "${src_path}/sys/system.cpp" "${src_path}/sys/systemproxy.cpp"
                        "${src_path}/sys/hw/espversion.cpp" "${src_path}/sys/hw/espsystem.cpp"
                        "${periph_path}/periphdrv.cpp" "${periph_path}/i2c.cpp" "${periph_path}/hw/espi2c.cpp"
                        "${periph_path}/uart.cpp" "${periph_path}/hw/espuart.cpp"
                    INCLUDE_DIRS "." "${src_path}/base" "${src_path}/sys" "${src_path}/sys/hw"
                        "${periph_path}" "${periph_path}/hw"
                    REQUIRES driver esp_adc_cal app_update nvs_flash)
