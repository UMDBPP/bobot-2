#ifndef _HELPERS_H
#define _HELPERS_H

// Libary list
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "hardware/gpio.h"
#include "hardware/i2c.h"
#include "hardware/irq.h"
#include "hardware/pwm.h"
#include "hardware/spi.h"
#include "pico/binary_info.h"
#include "pico/rand.h"
#include "pico/stdlib.h"
#include "rp2040-drf1262-lib/SX1262.h"
// #include "f_util.h"
// #include "ff.h"
// #include "hw_config.h"

// declare headers for C functions here
extern "C" {
#include "quadrature-decoder/quadrature_decoder.h"
}

// GPIO Declarations
#define SPIO_POCI 0
#define SPIO_CS_RAD 1
#define SPIO_SCK 2
#define SPIO_PICO 3
#define CROSS_PRIMTX_REDRX 4
#define CROSS_PRIMRX_REDTX 5
#define SDA 6
#define SCL 7
#define D1_pin 8
#define SPIO_CS_IMU 9
#define LS2 10
#define PWM_MC 11
#define GPS_TX 12
#define GPS_RX 13
#define LS1 14
#define CODER_1A 15  // Pin Base
#define CODER_1B 16  // Pin Base + 1
#define CODER_2A 17  // Pin Base
#define CODER_2B 18  // Pin Base + 1
#define CODER_3A 19  // Pin Base
#define CODER_3B 20  // Pin Base + 1
#define SPIO_CS_SD 21
#define SW_PIN 22
#define DIO1_PIN 23
#define BUSY_PIN 24
#define RST_PIN 25
#define TXEN_PIN 26
#define TIME_PULSE 27
#define STEP 28
#define DIRT 29

/*
I need to explain this next section so you understand why I used the extern
keyword. But to do that, I need to explain just a little bit about how this
source code gets turned into a final executable.

In essence the way that this project gets built is we have a number of .c or
.cpp source files with the code that we want to run. Some of it we write, some
of it is in libraries like the pico-sdk. To build all of these together in an
efficient way we break the process down into two steps.

1. Source Code -> Object file: this step takes the readable code we have and
turns it into machine code which is directly run by the machine (RP2040). This
can be summed up as .c or .cpp files -> .o files. You can find them in the build
folder but they won't be very readable.

2. Linking: Since we now have a bunch of machine code in different files we need
to bring it all together. This steps takes the .o files and bundles them into
our .elf and .uf2 files.

With that out of the way I'm going to explain extern now. Essentially, we want
files that include this header to be able to use the variables below. But since
the files are compiled separately, then linked, each .o files thinks it has the
one true copy of the variable (simplification but it should get the point
across). Since in C there can only be one one true copy, we have to tell the
linker to go find it when we try to bundle all of the files together. Thus, we
use the extern keyword to tell the linker that this file is using a variable
that is actually declared in another file.

In this case, the one true versons are in helpers.cpp (you can tell they because
they don't have extern in front). But files that include this header can use
them because they include a definition with extern in front, which signals to
the linker that the variable is declared elsewhere.
*/

// I2C address
const int addr_i2c = 0x77;

// variables declared in helpers.cpp
extern DRF1262 radio;
extern uint slice_num;
extern char radio_buf[];
extern quadrature_decoder coder1;
extern int32_t coder1_index;
extern quadrature_decoder coder2;
extern int32_t coder2_index;

// function prototypes
void init_pres(uint16_t *C1, uint16_t *C2, uint16_t *C3, uint16_t *C4,
               uint16_t *C5, uint16_t *C6);
void read_pres(uint16_t C1, uint16_t C2, uint16_t C3, uint16_t C4, uint16_t C5,
               uint16_t C6, int32_t *TEMP, int32_t *P);
void init_gps();
void read_gps();
int32_t quad_encoder_init1();
int32_t quad_encoder_init2();
uint PWM_init();
void motor_speed_dir_set(bool dir, int speed);
void teather_dis(bool dir, int dis, int speed);
void teather_dis(bool dir, int dis, int speed);
void gpio_callback(uint gpio, uint32_t events);

#endif