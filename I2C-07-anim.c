/* Many diagonal lines!!!. */

#include <stdint.h> //for data types
#include "registers.h" 
#include "i2c.h"
#include "uart.h"
#include "oled.h"
#include "ascii.h"

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

    display_framebuffer();

    while(1) {}
}

