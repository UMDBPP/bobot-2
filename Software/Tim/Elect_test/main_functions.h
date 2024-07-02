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
#include "../Elect_test/Libraries/quadrature-decoder/quadrature_decoder.h"
#include "../Libraries/rp2040-drf1262-lib/SX1262.h"
#include "ff.h"

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

//Radio class
DRF1262 radio(spi0, SPIO_CS_RAD, SPIO_SCK, SPIO_POCI, SPIO_PICO, TXEN_PIN, DIO1_PIN, BUSY_PIN, SW_PIN, RST_PIN);
//i2c address
const int addr_i2c = 0x77;
//Variables
uint slice_num;
char radio_buf[100] = {0};
//Function calls
void gpio_callback(uint gpio, uint32_t events);
//Function Definitions
void init_pres(uint16_t *C1, uint16_t *C2, uint16_t *C3, uint16_t *C4, uint16_t *C5, uint16_t *C6){
    //i2c initiate
    i2c_init(i2c1, 100 * 1000);//Start I2C controller 1
    gpio_set_function(SDA, GPIO_FUNC_I2C);//Set I2C pins to I2C mode
    gpio_set_function(SCL, GPIO_FUNC_I2C);
    gpio_pull_up(SDA);//Pull I2C pins up
    gpio_pull_up(SCL);
    bi_decl(bi_2pins_with_func(SDA, SCL, GPIO_FUNC_I2C));//Give binary info information about the I2C idk what this actually does

    //I2C commands
    uint8_t C1_cmd = 0xA2;//Read calibration C1
    uint8_t C2_cmd = 0xA4;//Read calibration C2
    uint8_t C3_cmd = 0xA6;//Read calibration C3
    uint8_t C4_cmd = 0xA8;//Read calibration C4
    uint8_t C5_cmd = 0xAA;//Read calibration C5
    uint8_t C6_cmd = 0xAC;//Read calibration C6
    //Calibtation constants
    uint8_t C1t[2];
    uint8_t C2t[2];
    uint8_t C3t[2];
    uint8_t C4t[2];
    uint8_t C5t[2];
    uint8_t C6t[2];

    i2c_write_blocking(i2c1, addr_i2c, &C1_cmd, sizeof(C1_cmd), false);//Send command to ask for C1 vale
    i2c_read_blocking(i2c1, addr_i2c, C1t, 2, false);//Read C1 value
    i2c_write_blocking(i2c1, addr_i2c, &C2_cmd, sizeof(C2_cmd), false);
    i2c_read_blocking(i2c1, addr_i2c, C2t, 2, false);
    i2c_write_blocking(i2c1, addr_i2c, &C3_cmd, sizeof(C3_cmd), false);
    i2c_read_blocking(i2c1, addr_i2c, C3t, 2, false);
    i2c_write_blocking(i2c1, addr_i2c, &C4_cmd, sizeof(C4_cmd), false);
    i2c_read_blocking(i2c1, addr_i2c, C4t, 2, false);
    i2c_write_blocking(i2c1, addr_i2c, &C5_cmd, sizeof(C5_cmd), false);
    i2c_read_blocking(i2c1, addr_i2c, C5t, 2, false);
    i2c_write_blocking(i2c1, addr_i2c, &C6_cmd, sizeof(C6_cmd), false);
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

void init_gps(){
    
    uart_init(uart0,9600);//Start UART0
    gpio_set_function(GPS_TX, GPIO_FUNC_UART);//Set GPS_TX as UART
    gpio_set_function(GPS_RX, GPIO_FUNC_UART);//Set GPS_RX as UART
    uart_set_hw_flow(uart0, false, false);//Turn of uart control pins

    //sample code to fill and print for GPS
    //uart_read_blocking(uart0, buffer_gps, 512);
    //printf("\n%s",buffer);
}


quadrature_decoder coder1;
int32_t coder1_index;
quadrature_decoder coder2;
int32_t coder2_index;

int32_t quad_encoder_init1()
{
    gpio_init(CODER_1A);
    gpio_init(CODER_1B);

    gpio_pull_down(CODER_1A);
    gpio_pull_down(CODER_1B);
    printf("%d", quadrature_decoder_init(&coder1, pio0));
    return add_quadrature_decoder(&coder1, CODER_1A);
}

int32_t quad_encoder_init2()
{
    gpio_init(CODER_2A);
    gpio_init(CODER_2B);

    gpio_pull_down(CODER_2A);
    gpio_pull_down(CODER_2B);
    printf("%d",quadrature_decoder_init(&coder2, pio1));
    return add_quadrature_decoder(&coder2, CODER_2A);
}

uint PWM_init(){
    uint slice_num;
    gpio_set_function(PWM_MC, GPIO_FUNC_PWM);
    slice_num = pwm_gpio_to_slice_num(PWM_MC);
    pwm_config config = pwm_get_default_config();
    pwm_config_set_clkdiv(&config, 16);
    pwm_config_set_wrap(&config, 19530);//400Hz
    pwm_init(slice_num, &config, true);
    pwm_set_chan_level(slice_num, 1, 0);
    return(slice_num);
}

void motor_speed_dir_set(bool dir, int speed){//0 is down 1 is up
    if(speed == 0){
        pwm_set_chan_level(slice_num, 1, 11750);
    }else if(dir){
        pwm_set_chan_level(slice_num, 1, (12000 + (speed * 33)));
    }else{
        pwm_set_chan_level(slice_num, 1, (11500 - (speed * 33)));
    }
}

void teather_dis(bool dir, int dis, int speed){//move dis distance in direction at speed
    //0 is down 1 is up
    int32_t start = get_count(&coder1, coder1_index);
    int32_t go = dis*1600;
    int32_t current;
    motor_speed_dir_set(dir, speed);
    while(1){
        current = get_count(&coder1, coder1_index);
        if((current >= (start + go))||(current <= (start - go))){
            break;
        }
    }
    motor_speed_dir_set(dir, 0);
}

void gpio_callback(uint gpio, uint32_t events) {
    if (gpio == DIO1_PIN) {
        radio.get_irq_status();

        if (radio.irqs.rx_done)
            radio.read_radio_buffer((uint8_t *)radio_buf, sizeof(radio_buf));

        if (radio.irqs.tx_done) radio.disable_tx();

        radio.clear_irq_status();
    }
}