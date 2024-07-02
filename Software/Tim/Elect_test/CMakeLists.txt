cmake_minimum_required(VERSION 3.13)

# initialize the SDK based on PICO_SDK_PATH
# note: this must happen before project()
include(pico_sdk_import.cmake)

project(Elect_test C CXX ASM)

set(CMAKE_C_STANDARD 11)

set(CMAKE_CXX_STANDARD 17)

# initialize the Raspberry Pi Pico SDK
pico_sdk_init()

# rest of your project
add_executable(main
    main.cpp
    ../Elect_test/Libraries/quadrature-decoder/quadrature_decoder.c
    ../Elect_test/Libraries/rp2040-drf1262-lib/SX1262.cpp
)
add_subdirectory(../Elect_test/Libraries/no-OS-FatFS-SD-SPI-RPi-Pico/FatFs_SPI build)

pico_generate_pio_header(main ${CMAKE_CURRENT_LIST_DIR}/Libraries/quadrature-decoder/quadrature_decoder.pio)

# Add libraries
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
    FatFs_SPI
    )

pico_enable_stdio_usb(main 1)
pico_enable_stdio_uart(main 0)

# create map/bin/hex/uf2 file in addition to ELF.
pico_add_extra_outputs(main)
