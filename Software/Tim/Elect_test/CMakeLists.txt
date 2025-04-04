# == DO NEVER EDIT THE NEXT LINES for Raspberry Pi Pico VS Code Extension to work ==
if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()
set(sdkVersion 2.0.0)
set(toolchainVersion 13_2_Rel1)
set(picotoolVersion 2.0.0)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if (EXISTS ${picoVscode})
    include(${picoVscode})
endif()
# ====================================================================================
# == DO NEVER EDIT THE NEXT LINES for Raspberry Pi Pico VS Code Extension to work ==
if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()
set(sdkVersion 2.0.0)
set(toolchainVersion 13_2_Rel1)
set(picotoolVersion 2.0.0)
include(${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
# ====================================================================================
cmake_minimum_required(VERSION 3.13)

# initialize the SDK based on PICO_SDK_PATH
# note: this must happen before project()
include(pico_sdk_import.cmake)

project(Elect_test C CXX ASM)

set(CMAKE_C_STANDARD 11)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# initialize the Raspberry Pi Pico SDK
pico_sdk_init()

# rest of your project
add_executable(main
    src/main.cpp
    src/helpers.cpp
    lib/quadrature-decoder/quadrature_decoder.c
    lib/rp2040-drf1262-lib/SX1262.cpp
    src/hw_config.c
)
add_subdirectory(../Elect_test/lib/no-OS-FatFS-SD-SDIO-SPI-RPi-Pico/src build)

pico_generate_pio_header(main ${CMAKE_CURRENT_LIST_DIR}/Lib/quadrature-decoder/quadrature_decoder.pio)

# Add libraries
target_link_directories(main PRIVATE src lib)

target_link_libraries(
    main 
    pico_stdlib
    pico_rand
    hardware_i2c
    hardware_spi
    hardware_pio
    hardware_dma
    hardware_pwm
    hardware_irq
    no-OS-FatFS-SD-SDIO-SPI-RPi-Pico
    )

pico_enable_stdio_usb(main 1)
pico_enable_stdio_uart(main 0)

# create map/bin/hex/uf2 file in addition to ELF.
pico_add_extra_outputs(main)
