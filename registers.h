#ifndef REGISTERS_H
#define REGISTERS_H

#include <stdint.h>

/* RCC register address low bound is 0x4002 3800 */
#define RCC_APB1ENR (*(volatile uint32_t *) 0x40023840)
#define RCC_AHB1ENR (*(volatile uint32_t *) 0x40023830)

/* GPIOA register address bounds. low bound = 0x40020000*/
#define GPIOA_MODER (*(volatile uint32_t *) 0x40020000)
#define GPIOA_AFRL (*(volatile uint32_t *) 0x40020020)

/* GPIOB register memory address low bound is 0x 0x4002 0400*/
#define GPIOB_MODER (*(volatile uint32_t *) 0x40020400)
#define GPIOB_AFRH (*(volatile uint32_t *) 0x40020424)
#define GPIOB_OTYPER (*(volatile uint32_t *) 0x40020404) //for setting pins to push-pull or open-drain

/* I2C register address low bound is 0x4000 5400 */
#define I2C_CR1 (*(volatile uint32_t *) 0x40005400)
#define I2C_CR2 (*(volatile uint32_t *) 0x40005404)
#define I2C_CCR (*(volatile uint32_t *) 0x4000541C)
#define I2C_TRISE (*(volatile uint32_t *) 0x40005420)
#define I2C_SR1 (*(volatile uint32_t *) 0x40005414)
#define I2C_SR2 (*(volatile uint32_t *) 0x40005418)
#define I2C_DR (*(volatile uint32_t *) 0x40005410)

/* USART Config */
#define USART_BRR (*(volatile uint32_t *) 0x40004408)
#define USART_CR1 (*(volatile uint32_t *)0x4000440C) 
#define USART_SR (*(volatile uint32_t *)0x40004400) 
#define USART_DR (*(volatile uint32_t *)0x40004404)

/* Nested Vectored Interrupt Controller */
#define NVIC_ISER1 (*(volatile uint32_t *) 0xE000E104) //ISER1 becuase it's position 38, and each ISER bus holds can hold 32 interrupt vectors 

/* For configuring SysTick */
#define SYST_CSR (*(volatile uint32_t *)0xE000E010) // control and status register
#define SYST_RVR (*(volatile uint32_t *)0xE000E014) // reload value register
#define SYST_CVR (*(volatile uint32_t *)0xE000E018) // current value register

#endif
