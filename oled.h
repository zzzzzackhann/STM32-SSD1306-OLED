#ifndef OLED_H
#define OLED_H

#include <stdint.h>

void ssd1306_oled_init(void);
void set_pixel(uint8_t x, uint8_t y);
void clear_framebuffer(void);
void display_framebuffer(void);

#endif
