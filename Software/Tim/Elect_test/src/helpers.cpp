#include "helpers.h"

//Radio class
DRF1262 radio(spi0, SPIO_CS_RAD, SPIO_SCK, SPIO_POCI, SPIO_PICO, TXEN_PIN, DIO1_PIN, BUSY_PIN, SW_PIN, RST_PIN);

//Variables
uint slice_num;
char radio_buf[100] = {0};
//Function calls
void gpio_callback(uint gpio, uint32_t events);
//Function Definitions
void init_pres(uint16_t *C1, uint16_t *C2, uint16_t *C3, uint16_t *C4, uint16_t *C5, uint16_t *C6){//Start I2C and retreive calibration constants
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
    //Calibtation constant arrays. I2C is read in 8 bit chunks each going to an array location to be concatenated later to create the full 16 bit number
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
    //Convert from 8 bit uint array to 16 bit uint
    *C1 = (C1t[1]<<8)|C1t[0];
    *C2 = (C2t[1]<<8)|C2t[0];
    *C3 = (C3t[1]<<8)|C3t[0];
    *C4 = (C4t[1]<<8)|C4t[0];
    *C5 = (C5t[1]<<8)|C5t[0];
    *C6 = (C6t[1]<<8)|C6t[0];
}

void read_pres(uint16_t C1, uint16_t C2, uint16_t C3, uint16_t C4, uint16_t C5, uint16_t C6, int32_t *TEMP, int32_t *P){//Takes Calibration constants and modifys presure and temperatuer variables
    //I2C commands
    uint8_t D1_cmd = 0x48;
    uint8_t D2_cmd = 0x58;
    uint8_t read = 0x00;
    //Variables
    //Temp variable arrays
    uint8_t D1t[3];
    uint8_t D2t[3];
    //Actual variables
    uint32_t D1;
    uint32_t D2;
    int32_t dT;
    int64_t OFF;
    int64_t SENS;

    i2c_write_blocking(i2c1, addr_i2c, &D1_cmd, sizeof(D1_cmd), false);//Write command for D1
    sleep_ms(100);//Let sensor retreive
    i2c_write_blocking(i2c1, addr_i2c, &read, sizeof(read), false);//Send read command
    i2c_read_blocking(i2c1, addr_i2c, D1t, 3, false);//Read data
    D1 =  0x00000000|(D1t[0]<<16)|(D1t[1]<<8)|D1t[2];//Place 24 bit pressure value in 32 bit uint from 3 reac bits

    i2c_write_blocking(i2c1, addr_i2c, &D2_cmd, sizeof(D2_cmd), false);//Write command for D2
    sleep_ms(100);//Let sensor retreive
    i2c_write_blocking(i2c1, addr_i2c, &read, sizeof(read), false);//Send read command
    i2c_read_blocking(i2c1, addr_i2c, D2t, 3, false);//Read data
    D2 = 0x00000000|(D2t[0]<<16)|(D2t[1]<<8)|D2t[2];//Place 24 bit pressure value in 32 bit uint from 3 reac bits

    dT = D2 - (C5 * 256);//Calc Temp diff
    *TEMP = 2000 + ((dT*C6)/8388608);//Calc temp
    
    OFF =  C2*131072 + ((C4*dT)/64);//Calc pressure offset
    SENS = (C1*65536) + ((C3*dT)/128);//Calculate sens
    *P = -(D1*SENS/2097152 - OFF)/32768;//Calculate pressure
}

void init_gps(){//Start UART for GPS
    
    uart_init(uart0,9600);//Start UART0
    gpio_set_function(GPS_TX, GPIO_FUNC_UART);//Set GPS_TX as UART
    gpio_set_function(GPS_RX, GPIO_FUNC_UART);//Set GPS_RX as UART
    uart_set_hw_flow(uart0, false, false);//Turn of uart control pins

    //sample code to fill and print for GPS
    //uart_read_blocking(uart0, buffer_gps, 512);
    //printf("\n%s",buffer);
}

void read_gps(){
    uint8_t *buffer_gps;
    buffer_gps = (uint8_t *)malloc(512);
    char * start;
    char * end;
    char * temp;
    char * message;
    uart_read_blocking(uart0, buffer_gps, 512);
    start = strstr((char *)buffer_gps, "$GNGGA");
    temp = strchr(start, ',');
    end = strchr(temp, '\n');
    message = (char *)malloc(end-start-1);
    strncpy(message, start, end-start-1);
    printf("\n\n%s", message);
    free(buffer_gps);
    free(message);
}

quadrature_decoder coder1;
int32_t coder1_index;
quadrature_decoder coder2;
int32_t coder2_index;

int32_t quad_encoder_init1(){//Set up encoder 1
    //GPIO init
    gpio_init(CODER_1A);
    gpio_init(CODER_1B);
    //Pull down GPIO pins
    gpio_pull_down(CODER_1A);
    gpio_pull_down(CODER_1B);
    printf("Encoder 1 good?: %d\n", quadrature_decoder_init(&coder1, pio0));
    return add_quadrature_decoder(&coder1, CODER_1A);
}

int32_t quad_encoder_init2(){//Set up encoder 2
    //GPIO init
    gpio_init(CODER_2A);
    gpio_init(CODER_2B);
    //pull down GPIO pins
    gpio_pull_down(CODER_2A);
    gpio_pull_down(CODER_2B);
    printf("Encoder 2 good: %d\n",quadrature_decoder_init(&coder2, pio1));
    return add_quadrature_decoder(&coder2, CODER_2A);
}

uint PWM_init(){//Init PWM for motor control
    //Variables
    uint slice_num;
    gpio_set_function(PWM_MC, GPIO_FUNC_PWM);//Setup GPIO pins
    slice_num = pwm_gpio_to_slice_num(PWM_MC);//Get PWM Slice number
    pwm_config config = pwm_get_default_config();//Get base config file
    pwm_config_set_clkdiv(&config, 16);//Modify config for clock scaling
    pwm_config_set_wrap(&config, 19530);//Modify config for time to wrap
    pwm_init(slice_num, &config, true);//Start PWM for given slice number using given slice number
    pwm_set_chan_level(slice_num, 1, 0);//Set PWM to 0
    return(slice_num);
}

void motor_speed_dir_set(bool dir, int speed){//Set motor to specific speed and direction. 0 is down 1 is up
    //PWM operates in a certian range with half of range as one direction other half as other. Small zone in the middle for no movement
    if(speed == 0){//If 0 have motor hold still
        pwm_set_chan_level(slice_num, 1, 11750);//Set PWM to center value
    }else if(dir){//Choose direction
        pwm_set_chan_level(slice_num, 1, (12000 + (speed * 33)));//Set PWM to scale of speed above center
    }else{
        pwm_set_chan_level(slice_num, 1, (11500 - (speed * 33)));//Set PWM to scale of speed below the center
    }
}

void teather_dis(bool dir, int dis, int speed){//move distance in direction at speed
    //0 is down 1 is up
    int32_t start = get_count(&coder1, coder1_index);//Retrieve starting position from encoder
    int32_t go = dis*1900;//Calculate number of encoder steps to go
    int32_t current;//Variable for current encoder count
    motor_speed_dir_set(dir, speed);//Start motor
    while(1){
        current = get_count(&coder1, coder1_index);//Get current location
        if((current >= (start + go))||(current <= (start - go))){//Compare current to offset of go in either direction from start
            break;//exit loop
        }
    }
    motor_speed_dir_set(dir, 0);//Stop motor
}

void gpio_callback(uint gpio, uint32_t events) {//Radio interupt callback
    if (gpio == DIO1_PIN) {
        radio.get_irq_status();

        if (radio.irqs.rx_done)
            radio.read_radio_buffer((uint8_t *)radio_buf, sizeof(radio_buf));

        if (radio.irqs.tx_done) radio.disable_tx();

        radio.clear_irq_status();
    }
}
/*
void init_sd(FATFS fs, FIL fil, FRESULT *fr, const char* const filename){//initalize SD card
    *fr = f_mount(&fs, "", 1);//mount SD card
    if (FR_OK != *fr) panic("f_mount error: %s (%d)\n", FRESULT_str(*fr), *fr);//check if it mounted correctly
    
    *fr = f_open(&fil, filename, FA_OPEN_APPEND | FA_WRITE);//Open file or create if no file
    if (FR_OK != *fr && FR_EXIST != *fr)//check if file is open correctly
        panic("f_open(%s) error: %s (%d)\n", filename, FRESULT_str(*fr), *fr);
    if (f_printf(&fil, "Bobot-2 data log\n\n") < 0) {//Put header in file
        printf("f_printf failed\n");
    }
    *fr = f_close(&fil);//Close file
    if (FR_OK != *fr) {//Check if file is closed
        printf("f_close error: %s (%d)\n", FRESULT_str(*fr), *fr);
    }
    f_unmount("");//unmount SD card
}

void sd_write(FATFS fs, FIL fil, FRESULT *fr, const char* const filename, int32_t P, int32_t TEMP){//Put information onto SD card
    *fr = f_mount(&fs, "", 1);//mount SD card
    if (FR_OK != *fr) panic("f_mount error: %s (%d)\n", FRESULT_str(*fr), *fr);//Check if it mounted correctly
    
    *fr = f_open(&fil, filename, FA_OPEN_APPEND | FA_WRITE);//Open file
    if (FR_OK != *fr && FR_EXIST != *fr)//check if file is open correctly
        panic("f_open(%s) error: %s (%d)\n", filename, FRESULT_str(*fr), *fr);
    if (f_printf(&fil, "Temperature: %d, Pressure: %d\n", TEMP, P) < 0) {//Write temperatuer and pressure to SD card and confirm write operation
        printf("f_printf failed\n");
    }
    *fr = f_close(&fil);//Close file
    if (FR_OK != *fr) {//Check if file closed correctly
        printf("f_close error: %s (%d)\n", FRESULT_str(*fr), *fr);
    }
    f_unmount("");//unmount SD card
}
*/