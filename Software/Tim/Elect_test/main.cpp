#include "main_functions.h"

#include <stdio.h>
#include "hardware/i2c.h"
#include "pico/stdlib.h"
#include "pico/binary_info.h"


void init_pres(uint16_t *C1, uint16_t *C2, uint16_t *C3, uint16_t *C4, uint16_t *C5, uint16_t *C6);
void read_pres(uint16_t C1, uint16_t C2, uint16_t C3, uint16_t C4, uint16_t C5, uint16_t C6, int32_t *TEMP, int32_t *P);
void init_spi();


int main() {
    //Variables
    uint16_t C1;
    uint16_t C2;
    uint16_t C3;
    uint16_t C4;
    uint16_t C5;
    uint16_t C6;
    int32_t TEMP;
    int32_t P;

    uint8_t *buffer_gps;
    buffer_gps = (uint8_t *)malloc(512);
    
    slice_num = PWM_init();

    pwm_set_enabled(slice_num, true);
    stdio_init_all();
    init_pres(&C1, &C2, &C3, &C4, &C5, &C6);
    init_gps();
    sleep_ms(3000);
    coder1_index = quad_encoder_init1();
    coder2_index = quad_encoder_init2();
    

    gpio_set_irq_callback(&gpio_callback);
    sleep_ms(5000);
    // enables debug print statements in radio methods
    radio.debug_msg_en = 1;

    radio.radio_init();

    while (1){
        printf("Transmit Test\n");

        char data[] = "sendx";

        data[4] = (char)get_rand_32();

        radio.radio_send((uint8_t *)data, sizeof(data));
        sleep_ms(1000);
    }

    return(0);
}