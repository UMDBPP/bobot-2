#include "main_functions.h"
#include <stdio.h>
#include "hardware/i2c.h"
#include "pico/stdlib.h"
#include "pico/binary_info.h"

void init_pres(uint16_t *C1, uint16_t *C2, uint16_t *C3, uint16_t *C4, uint16_t *C5, uint16_t *C6);
void read_pres(uint16_t C1, uint16_t C2, uint16_t C3, uint16_t C4, uint16_t C5, uint16_t C6, int32_t *TEMP, int32_t *P);

uint8_t reverseBits(uint8_t num);

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

    stdio_init_all();
    init_pres(&C1, &C2, &C3, &C4, &C5, &C6);
    
    while (true) {
        read_pres(C1, C2, C3, C4, C5, C6, &TEMP, &P);
        printf("Temp: %d, Pressure: %d\n",TEMP, P);
        sleep_ms(1000);
    }
}