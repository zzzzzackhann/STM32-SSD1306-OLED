/* Many diagonal lines!!!. */

#include <stdint.h> //for data types
#include "registers.h" 
#include "i2c.h"
#include "uart.h"
#include "oled.h"

void clock_enable(void) {
    RCC_APB1ENR |= (1 << 21) | (1 << 17); // I2C1 and USART 2
    RCC_AHB1ENR |= (1 << 0) | (1 << 1); // GPIOA and GPIOB
}

int main(void) {
    clock_enable();
    i2c_init();
    usart_init();
    i2c_check_addresses();
    ssd1306_oled_init();
    ssd1306_set_draw_window();  

    clear_framebuffer();

    for(uint8_t i = 0; i < 128; i += 10) {
        for(uint8_t j = 0; j < 64; j++) {
            uint16_t x = j * 2 + i; 
            if(x < 128) {
                set_pixel(x, j);
            }
        } 
    }

    for(uint8_t i = 0; i < 64; i++) {
        for(uint8_t j = 0; j < 64; j += 5) {
            uint16_t y = j + i; 
            if(y < 64){
                set_pixel(i * 2, y);
            }  
        } 
    }

    display_framebuffer();

    while(1) {}
}

