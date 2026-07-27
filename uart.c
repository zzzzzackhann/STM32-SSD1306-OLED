#include "registers.h"

void usart_init(void) {
    GPIOA_MODER &= ~(15 << 4); 
    GPIOA_MODER |= (2 << 4) | (2 << 6); // set PA2 and PA3 to alternate function 

    GPIOA_AFRL &= ~((15 << 8) | (15 << 12));
    GPIOA_AFRL |= (7 << 8) | (7 << 12); // set PA2 and P3 to alt func 7, which is USART2

    USART_BRR = (8 << 4) | 11; //that crazy baud rate mantissa/fraction setup (baud rate is 115200)

    NVIC_ISER1 |= (1 << 6); //bit 6 on ISER1 for interrupt 38 (each ISER bus carries 32 interrupts)
    
    USART_CR1 |= (1 << 3) | (1 << 2) | (1 << 5) | (1 << 13); //TX enable, RX enable, RX interrupt enable, UART unit enable
}

void usart_send_char(char c) {
    while(!(USART_SR & (1 << 7))); // wait until buffer is empty
    USART_DR = c; // fill buffer with char
}

void usart_send_string(char* str) {
    while(*str) {
        usart_send_char(*str);
        str++;
    }
}

void usart_send_num(uint8_t num) {
    uint8_t high_val = (num >> 4); // 00111100 becomes 00000011, or 3
    uint8_t low_val = (num & 0x0F); // 00111100 becomes 00001100, or 12 (C)

    char hex_vals[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};

    char buf[3];

    buf[2] = '\0';
    buf[1] = hex_vals[low_val];
    buf[0] = hex_vals[high_val];

    usart_send_string("0x");
    usart_send_string(buf);
}

void usart2_handler(void) {
    usart_send_char(USART_DR);
}
