# STM32 OLED display driver

A bare-metal STM32 driver for a 128x64 SSD1306 OLED display. Written in C with direct register manipulation and modular file organization. 

## What I built

- Initialization scripts on the STM32F446RE Nucleo Board for USART, I2C, interrupts, and the SSD1306 OLED display   

- Debugging functions through UART and STLink

- A framebuffer mapped to the OLED's pixel display, with functions for writing into and clearing the buffer

## What I learned

- Initialization sequence for I2C on STM32

- SSD1306 init sequence, pixel addressing modes, bump charge for driving LEDs

- functions for drawing basic shapes on 128x64 display
