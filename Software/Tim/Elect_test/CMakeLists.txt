cmake_minimum_required(VERSION 3.13)

# initialize the SDK based on PICO_SDK_PATH
# note: this must happen before project()
include(./lib/pico-sdk/pico_sdk_init.cmake)

project(Elect_test C CXX ASM)

# initialize the Raspberry Pi Pico SDK
pico_sdk_init()

# All of the .c and .cpp files from src and lib that need to be compiled.
add_executable(test_project
    src/main.cpp
    src/helpers.cpp
    lib/quadrature-decoder/quadrature_decoder.c
    lib/rp2040-drf1262-lib/SX1262.cpp
)

# This line adds src and lib as include directories. This means that if a file from src wants to 
# include files from the DRF1262 library in lib, it can just #include "rp2040-drf1262-lib/SX1262.h" 
# instead of #include "../lib/rp2040-drf1262-lib/SX1262.h"
target_include_directories(test_project PRIVATE src lib)

# Add the .pio needed for quadrature decoding to the build
pico_generate_pio_header(test_project lib/quadrature-decoder/quadrature_decoder.pio)

# Add libraries, these are portions of the SDK needed for certain functionality, generally when you #include a 
# header file from the SDK you need to add the library here for it to get compiled correctly
target_link_libraries(test_project 
    pico_stdlib
    pico_rand
    hardware_i2c
    hardware_spi
    hardware_pio
    hardware_dma
    hardware_pwm
    hardware_irq
)

# These lines either enable (1) or disable (0) USB and UART0 at stdout outputs. 
# Functionally, these are used to select if you want printf() to print to USB, UART0, or both. 
# In general you want printf to go to USB since that's how you'll be connected to upload code. 
# For Bobot-2 boards you don't want printf() to go to UART0 since that's being used for other functions.
pico_enable_stdio_usb(test_project 1)
pico_enable_stdio_uart(test_project 0)

# Create map/bin/hex/uf2 file in addition to ELF. These are all things you might want, UF2 especially.
pico_add_extra_outputs(test_project)
