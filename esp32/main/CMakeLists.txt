idf_component_register(
    SRCS "main.cpp" "../../src/module.cpp" "../../src/system.cpp" "../../src/systemproxy.cpp" "../../src/time.cpp" "../../src/version.cpp" "../../src/hw/espsystem.cpp" "../../src/hw/espversion.cpp"
    INCLUDE_DIRS "." "../../src/" "../../src/hw/"
)
