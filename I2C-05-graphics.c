/* Added framebuffer and functions to push framebuffer to display. Displayed a diagonal line. */

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

    uint8_t memory_addressing_config[] = {0x00, 0x20, 0x20}; // for horizontal memory addressing
    uint8_t column_address_range[] = {0x00, 0x21, 0, 127}; //sets column address range of 0-127 (whole display)
    uint8_t page_address_range[] = {0x00, 0x22, 0, 7}; // for pages 0-8

    i2c_write(0x3C, memory_addressing_config, sizeof(memory_addressing_config) / sizeof(memory_addressing_config[0]));
    i2c_write(0x3C, column_address_range, sizeof(column_address_range) / sizeof(column_address_range[0]));
    i2c_write(0x3C, page_address_range, sizeof(page_address_range) / sizeof(page_address_range[0]));

    
    clear_framebuffer();

    for(uint8_t i = 0; i < 64; i++) {
        set_pixel(i * 2, i);
    }

    display_framebuffer();

    while(1) {}
}
