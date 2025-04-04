#include "helpers.h"

#include <stdio.h>
#include "hardware/i2c.h"
#include "pico/stdlib.h"
#include "pico/binary_info.h"


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

    
    //Get slice num for PWM
    slice_num = PWM_init();

    pwm_set_enabled(slice_num, true);//enable PWM
    stdio_init_all();//Initalize IO
    init_pres(&C1, &C2, &C3, &C4, &C5, &C6);//Initalize pressure temperature
    init_gps();//Initalize GPS
    sleep_ms(3000);
    coder1_index = quad_encoder_init1();//Initalize encoders
    coder2_index = quad_encoder_init2();
    

    gpio_set_irq_callback(&gpio_callback);
    sleep_ms(5001);
    // enables debug print statements in radio methods
    radio.debug_msg_en = 1;

    radio.radio_init();
    //SD card object
    FATFS fs;
    FIL fil;
    FRESULT fr;
    const char* const filename = "Bobot-2-data.txt";
    init_sd(fs, fil, &fr, filename);
       

    gpio_init(SPIO_CS_IMU);
    gpio_set_dir(SPIO_CS_IMU, GPIO_OUT);
    gpio_put(SPIO_CS_IMU, 1);

    uint16_t imu_dummy_byte = 0x00; 
    uint8_t imu_buffer[2];
    spi_read_blocking(spi0, imu_dummy_byte, imu_buffer, sizeof(imu_buffer));
    printf("Wellll: %d", imu_buffer[0]);

    while (1){
        //sd_write(fs, fil, &fr, filename, P, TEMP);
        read_pres(C1,C2,C3,C4,C5,C6,&TEMP,&P);
        printf("Temp: %d Pressure: %d \n", TEMP, P);

        sleep_ms(1000);
    }

    return(0);
}