/* Many diagonal lines!!!. */

#include <stdint.h> //for data types
#include "registers.h" 
#include "i2c.h"
#include "uart.h"
#include "oled.h"
#include "ascii.h"
// #include "systick.h"

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
    // systick_init();

    int8_t dx = 2;
    uint8_t dy = 2;
    uint8_t x = 0;
    uint8_t y = 0;
    
    while(1) {

        clear_framebuffer();

        for(uint8_t i = 0; i < 20; i++){
            set_pixel(i + x, 0 + y);
            set_pixel(i + x , 20 + y);
            set_pixel(0 + x, i + y);
            set_pixel(20 + x, i + y);
        }
        
        x += dx;
        y += dy;


        if(x <=0 || x >= 106) dx = -dx;
        if(y <=0 || y >= 42) dy = -dy;

        display_framebuffer();
    }
}

