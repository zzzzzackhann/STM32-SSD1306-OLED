# STM32 OLED display driver

A bare-metal STM32 driver for a 128x64 SSD1306 OLED display. Written in C with direct register manipulation and modular file organization. 
Demo code includes text display, basic graphics display, and basic animations.   

## What I built

- Initialization scripts on the STM32F446RE Nucleo Board for USART, I2C, interrupts, and the SSD1306 OLED display   
- Debugging functions through UART and STLink
- A framebuffer mapped to the OLED's pixel display, with functions for writing into and clearing the buffer
- Functions for creating basic shapes and projecting text onto display
- Functions to animate graphics and text

## What I learned

- Initialization sequence for I2C on STM32
- SSD1306 init sequence, pixel addressing modes, bump charge for driving LEDs
- Refactoring functions into linked header files 
- Functions for drawing basic graphics on 128x64 display
- Nested for loops for duplicating basic shapes across the display
- Bitmapping character values in a nested array for display text on a pixel display
- Algorithms for animating objects on a display

## What I would do differently
- Add hardware controls for manipulating graphics
- Expand letters_array to include lowercase letters or other fonts
- Create a function for changing font size 
- Create more complex shapes
