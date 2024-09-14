cmake_minimum_required(VERSION 3.13)

# initialize the SDK based on PICO_SDK_PATH
# note: this must happen before project()
include(./lib/pico-sdk/pico_sdk_init.cmake)

project(Elect_test C CXX ASM)

# set(CMAKE_C_STANDARD 11)

# set(CMAKE_CXX_STANDARD 17)

# set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# initialize the Raspberry Pi Pico SDK
pico_sdk_init()



# rest of your project
add_executable(main
    src/main.cpp
    src/helpers.cpp
    lib/quadrature-decoder/quadrature_decoder.c
    lib/rp2040-drf1262-lib/SX1262.cpp
    #hw_config.c
)
# add_subdirectory(../Elect_test/Libraries/no-OS-FatFS-SD-SDIO-SPI-RPi-Pico/src build)

target_include_directories(main PRIVATE src lib)

pico_generate_pio_header(main ${CMAKE_CURRENT_LIST_DIR}/lib/quadrature-decoder/quadrature_decoder.pio)

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
    # no-OS-FatFS-SD-SDIO-SPI-RPi-Pico
    )

pico_enable_stdio_usb(main 1)
pico_enable_stdio_uart(main 0)

# create map/bin/hex/uf2 file in addition to ELF.
pico_add_extra_outputs(main)
