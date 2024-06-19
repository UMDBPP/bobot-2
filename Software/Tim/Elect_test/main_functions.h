#include <stdio.h>
#include "hardware/i2c.h"
#include "pico/stdlib.h"
#include "pico/binary_info.h"

//GPIO Declarations
const int SPIO_POCI = 0;
const int SPIO_CS_RAD = 1;
const int SPIO_SCK = 2;
const int SPIO_PICO = 3;
const int CROSS_PRIMTX_REDRX = 4;
const int CROSS_PRIMRX_REDTX = 5;
const int SDA = 6;
const int SCL = 7;
const int D1 = 8;
const int SPIO_CS_IMU = 9;
const int LS2 = 10;
const int PWM_MC = 11;
const int GPS_RX = 12;
const int GPS_TX = 13;
const int LS1 = 14;
const int CH_1A = 15;
const int CH_1B = 16;
const int CH_2A = 17;
const int CH_2B = 18;
const int CH_3A = 19;
const int CH_3B = 20;
const int SPIO_CS_SD = 21;
const int SW = 22;
const int DIO1 = 23;
const int BUSY = 24;
const int RST = 25;
const int TXEN = 26;
const int TIME_PULSE = 27;
const int STEP = 28;
const int DIRT = 29;
//i2c address
const int addr_i2c = 0x77;

void init_pres(uint16_t *C1, uint16_t *C2, uint16_t *C3, uint16_t *C4, uint16_t *C5, uint16_t *C6){
    //i2c initiate
    i2c_init(i2c1, 100 * 1000);
    gpio_set_function(SDA, GPIO_FUNC_I2C);
    gpio_set_function(SCL, GPIO_FUNC_I2C);
    gpio_pull_up(SDA);
    gpio_pull_up(SCL);
    bi_decl(bi_2pins_with_func(SDA, SCL, GPIO_FUNC_I2C));

    //I2C commands
    uint8_t C1_cmd = 0xA2;
    uint8_t C2_cmd = 0xA4;
    uint8_t C3_cmd = 0xA6;
    uint8_t C4_cmd = 0xA8;
    uint8_t C5_cmd = 0xAA;
    uint8_t C6_cmd = 0xAC;
    //Calibtation constants
    uint8_t C1t[2];
    uint8_t C2t[2];
    uint8_t C3t[2];
    uint8_t C4t[2];
    uint8_t C5t[2];
    uint8_t C6t[2];

    i2c_write_blocking(i2c1, addr_i2c, &C1_cmd, sizeof(C1_cmd), false);
    i2c_read_blocking(i2c1, addr_i2c, C1t, 2, false);
    i2c_write_blocking(i2c1, addr_i2c, &C2_cmd, sizeof(C1_cmd), false);
    i2c_read_blocking(i2c1, addr_i2c, C2t, 2, false);
    i2c_write_blocking(i2c1, addr_i2c, &C3_cmd, sizeof(C1_cmd), false);
    i2c_read_blocking(i2c1, addr_i2c, C3t, 2, false);
    i2c_write_blocking(i2c1, addr_i2c, &C4_cmd, sizeof(C1_cmd), false);
    i2c_read_blocking(i2c1, addr_i2c, C4t, 2, false);
    i2c_write_blocking(i2c1, addr_i2c, &C5_cmd, sizeof(C1_cmd), false);
    i2c_read_blocking(i2c1, addr_i2c, C5t, 2, false);
    i2c_write_blocking(i2c1, addr_i2c, &C6_cmd, sizeof(C1_cmd), false);
    i2c_read_blocking(i2c1, addr_i2c, C6t, 2, false);

    *C1 = (C1t[1]<<8)|C1t[0];
    *C2 = (C2t[1]<<8)|C2t[0];
    *C3 = (C3t[1]<<8)|C3t[0];
    *C4 = (C4t[1]<<8)|C4t[0];
    *C5 = (C5t[1]<<8)|C5t[0];
    *C6 = (C6t[1]<<8)|C6t[0];
}

void read_pres(uint16_t C1, uint16_t C2, uint16_t C3, uint16_t C4, uint16_t C5, uint16_t C6, int32_t *TEMP, int32_t *P){
    uint8_t D1_cmd = 0x48;
    uint8_t D2_cmd = 0x58;
    uint8_t read = 0x00;
    //Variables
    uint8_t D1t[3];
    uint8_t D2t[3];
    uint32_t D1;
    uint32_t D2;
    int32_t dT;
    int64_t OFF;
    int64_t SENS;

    i2c_write_blocking(i2c1, addr_i2c, &D1_cmd, sizeof(D1_cmd), false);
    sleep_ms(100);
    i2c_write_blocking(i2c1, addr_i2c, &read, sizeof(read), false);
    i2c_read_blocking(i2c1, addr_i2c, D1t, 3, false);
    D1 =  0x00000000|(D1t[0]<<16)|(D1t[1]<<8)|D1t[2];

    i2c_write_blocking(i2c1, addr_i2c, &D2_cmd, sizeof(D2_cmd), false);
    sleep_ms(100);
    i2c_write_blocking(i2c1, addr_i2c, &read, sizeof(read), false);
    i2c_read_blocking(i2c1, addr_i2c, D2t, 3, false);
    D2 = 0x00000000|(D2t[0]<<16)|(D2t[1]<<8)|D2t[2];

    dT = D2 - (C5 * 256);
    *TEMP = 2000 + ((dT*C6)/8388608);
    
    OFF =  C2*131072 + ((C4*dT)/64);
    SENS = (C1*65536) + ((C3*dT)/128);
    *P = -(D1*SENS/2097152 - OFF)/32768;
}