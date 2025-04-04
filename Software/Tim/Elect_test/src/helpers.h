#ifndef _HELPERS_H
#define _HELPERS_H

//Libary list
#include <stdio.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include <string.h>
#include <math.h>
#include "pico/stdlib.h"
#include "pico/binary_info.h"
#include "pico/rand.h"
#include "hardware/i2c.h"
#include "hardware/spi.h"
#include "hardware/pwm.h"
#include "hardware/gpio.h"
#include "hardware/irq.h"
#include "../lib/rp2040-drf1262-lib/SX1262.h"
#include "f_util.h"
#include "ff.h"
#include "hw_config.h"

extern "C" {
#include "../lib/quadrature-decoder/quadrature_decoder.h"
}

//GPIO Declarations
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
#define CODER_1B 16 // Pin Base + 1
#define CODER_2A 17  // Pin Base
#define CODER_2B 18 // Pin Base + 1
#define CODER_3A 19  // Pin Base
#define CODER_3B 20 // Pin Base + 1
#define SPIO_CS_SD 21
#define SW_PIN 22
#define DIO1_PIN 23
#define BUSY_PIN 24
#define RST_PIN 25
#define TXEN_PIN 26
#define TIME_PULSE 27
#define STEP 28
#define DIRT 29

//i2c address
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
void init_pres(uint16_t *C1, uint16_t *C2, uint16_t *C3, uint16_t *C4, uint16_t *C5, uint16_t *C6);
void read_pres(uint16_t C1, uint16_t C2, uint16_t C3, uint16_t C4, uint16_t C5, uint16_t C6, int32_t *TEMP, int32_t *P);
void init_gps();
void read_gps();
int32_t quad_encoder_init1();
int32_t quad_encoder_init2();
uint PWM_init();
void motor_speed_dir_set(bool dir, int speed);
void teather_dis(bool dir, int dis, int speed);
void gpio_callback(uint gpio, uint32_t events);
void init_sd(FATFS fs, FIL fil, FRESULT *fr, const char* const filename);
void sd_write(FATFS fs, FIL fil, FRESULT *fr, const char* const filename, int32_t P, int32_t TEMP);
void init_imu();

#endif