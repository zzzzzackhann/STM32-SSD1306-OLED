#include <stdint.h>
#include "i2c.h"

static const uint8_t oled_init_seq[] = {
    0x00, // control byte
    0xAE, // turn display off
    0xA8, 0x3F, // set Multiplex ratio
    0xD3, 0x00, // set display offset
    0x40, // set display start line
    0xA0, // map column 0 (x = 0) to SEG0
    0xC0, // set COM output scan direction
    0xDA, 0x12, // set COM pins for alternative hardware config, which Claude says is necessary for 128x64 screen
    0x81, 0x7F, // set contrast control (this is the default)
    0xA4, // entire display ON, output follows RAM content
    0xA6, // sets normal display (not inverted)
    0xD5, 0xF0, // define internal clock (CLK, set to max (0xF)) and display divider (D, set to min (0x0)_
    0x8D, 0x14, // enable charge pump regulator
    0xAF // display ON
};

void ssd1306_oled_init(void) {
    uint8_t len = sizeof(oled_init_seq) / sizeof(oled_init_seq[0]);
    i2c_write(0x3C, oled_init_seq, len);
}

static uint8_t framebuffer[1024];

void set_pixel(uint8_t x, uint8_t y) {
    uint16_t byte_index = y/8 * 128 + x; // index in 1024 bit array that follows horizontal addressing pattern
    uint8_t bit = y % 8; // which bit (pixel) in the byte (column)
    framebuffer[byte_index] |= (1 << bit); // turn that pixel on
}

void clear_framebuffer(void) {
    for(uint16_t i = 0; i < 1024; i++) {
        framebuffer[i] = 0;
    }
}

void display_framebuffer(void) {
    uint8_t cmd[1025];
    cmd[0] = 0x40; // send pixel data
    for(uint16_t i = 1; i <= 1024; i++){
        cmd[i] = framebuffer[i-1];
    }

    i2c_write(0x3C, cmd, 1025);
}

static const uint8_t memory_addressing_config[] = {0x00, 0x20, 0x20}; // for horizontal memory addressing
static const uint8_t column_address_range[] = {0x00, 0x21, 0, 127}; //sets column address range of 0-127 (whole display)
static const uint8_t page_address_range[] = {0x00, 0x22, 0, 7}; // for pages 0-8

void ssd1306_set_draw_window(void){
    i2c_write(0x3C, memory_addressing_config, sizeof(memory_addressing_config) / sizeof(memory_addressing_config[0]));
    i2c_write(0x3C, column_address_range, sizeof(column_address_range) / sizeof(column_address_range[0]));
    i2c_write(0x3C, page_address_range, sizeof(page_address_range) / sizeof(page_address_range[0]));
}
