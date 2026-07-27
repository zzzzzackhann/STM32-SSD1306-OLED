#include <stdint.h> //for data types

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

void clock_enable(void) {
    RCC_APB1ENR |= (1 << 21) | (1 << 17); // I2C1 and USART 2
    RCC_AHB1ENR |= (1 << 0) | (1 << 1); // GPIOA and GPIOB

}

void i2c_init(void) {
    /* Configure PB8 and PB9 for alternate function */
    GPIOB_MODER &= ~(15 << 16); 
    GPIOB_MODER |= (1 << 17) | (1 << 19); // alternate function mode 
    GPIOB_AFRH &= ~255;
    GPIOB_AFRH |= (4 << 0) | (4 << 4); // AF4 (I2C1)
    GPIOB_OTYPER |= (3 << 8); // set PB8 and PB9 to output open-drain

    /* Set and clear I2C reset bit */
    I2C_CR1 |= (1 << 15);
    I2C_CR1 &= ~(1 << 15); 

    /* Set I2C freq */
    I2C_CR2 = 16; // APB1_CLK / I2C_FREQ
    I2C_CCR = 80; // value for 100kHz I2C clock with 16MHz peripheral clock
    I2C_TRISE = 17; // PCLK1_MHz + 1
    I2C_CR1 = 1; // last thing - ENABLE the peripheral
}

void usart_init(void) {
    GPIOA_MODER &= ~(15 << 4); // clear bits for PA2 and PA3
    GPIOA_MODER |= (2 << 4) | (2 << 6);

    GPIOA_AFRL &= ~((15 << 8) | (15 << 12));
    GPIOA_AFRL |= (7 << 8) | (7 << 12);

    USART_BRR = (8 << 4) | 11;

    NVIC_ISER1 |= (1 << 6); //bit 6 for interrupt 38 on bus ISER1
    
    USART_CR1 |= (1 << 3) | (1 << 2) | (1 << 5) | (1 << 13); //TX enable, RX enable, RX interrupt enable, UART unit enable
}

void systick_init(void) {
    SYST_RVR = 15999;
    SYST_CVR = 0;
    SYST_CSR |= 1 | (1 << 2);
}

void usart_send_char(char c) {
    while(!(USART_SR & (1 << 7))); //wait until buffer is empty
    USART_DR = c;
}

void usart_send_string(char* str){
    while(*str) {
        usart_send_char(*str);
        str++;
    }
}

void i2c_check_address(uint32_t addr) {
    I2C_CR1 |= (1 << 8); // send start bit
    while(!(I2C_SR1 & (1 << 0)));  //wait until start bit sets to 1, meaning the info is on the bus
    I2C_DR = (addr << 1) | 0; // 0 is the 0 position bit: 0 = write, 1 = read; in this case asking for I2C address 0

    while(!(I2C_SR1 & (1 << 1)) && !(I2C_SR1 & (1 << 10))); //wait until either address acknowledged or failure acknowledged
    uint8_t present = (I2C_SR1 & (1 << 1)) ? 1 : 0; // save whether address was acknowledged or failed 
    uint32_t temp = I2C_SR2; //have to read I2C_SR2 to clear ADDR flag
    (void) temp;

    if(present == 1) {
            usart_send_string("I2C device is present.\r\n");
        } else {
            I2C_SR1 &= ~(1 << 10); // have to manually clear AF if flag is set
            usart_send_string("Handshake failed. I2C device is absent.\r\n");
           
        }

    I2C_CR1 |= (1 << 9); // set stop flag (it's then automatically cleared by the hardware)
}

void usart2_handler(void) {
    usart_send_char(USART_DR);
}

void delay_ms(uint16_t ms) {
    for (uint16_t i = 0; i < ms; i++) {
        while(!(SYST_CSR & (1 << 16)));
    }
}

int main(void) {
    clock_enable();
    i2c_init();
    usart_init();
    systick_init();

    while(1) {
        i2c_check_address(0x3d);
        delay_ms(5000);
    }

}
