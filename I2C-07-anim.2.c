/* Many diagonal lines!!!. */

#include <stdint.h> // for data types
#include "registers.h" 
#include "i2c.h"
#include "uart.h"
#include "oled.h"
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
    int8_t dy = 4;
    int8_t x = 0;
    int8_t y = 0;

    char string[] = "I love you Sofi :)";
    
    while(1) {

        clear_framebuffer();
        draw_string(string, x, y);

        x += dx;
        y += dy;

        if(x <=0 || x >= (127 - ((sizeof(string) * 6)))) dx = -dx; // number of chars in string[], plus 1 for the null character, 6 pixels each (5 for character, 1 for space)
        if(y <=0 || y >= 55) dy = -dy; // size of frame(y) which is 63, minus height of chacaters (8)

        display_framebuffer();
    }
}

/* Order of operations really matters here. First, run the program. The check the values for the next loop, adjust the values, and loop.  */
