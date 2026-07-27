#ifndef UART_H
#define UART_H

#include <stdint.h>

void usart_init(void);
void usart_send_char(char c);
void usart_send_string(char* str);
void usart_send_num(uint8_t num);
void usart2_handler(void);

#endif
