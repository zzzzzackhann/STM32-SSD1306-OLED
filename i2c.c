#include <stdint.h>
#include "registers.h"
#include "uart.h"

void i2c_init(void) {
    /* Configure PB8 and PB9 for alternate function */
    GPIOB_MODER &= ~(15 << 16); 
    GPIOB_MODER |= (1 << 17) | (1 << 19); // sets alternate function mode for PB8 and PB9
    GPIOB_AFRH &= ~255;
    GPIOB_AFRH |= (4 << 0) | (4 << 4); // sets alt func 4 (I2C1) for PB8 and PB 9
    GPIOB_OTYPER |= (3 << 8); // set PB8 and PB9 to output open-drain, because I2C is open drain

    /* Set and clear I2C reset bit */
    I2C_CR1 |= (1 << 15); // sets reset bit
    I2C_CR1 &= ~(1 << 15);  //clears reset bit

    /* Set I2C freq */
    I2C_CR2 = 16; // peripheral clock freq in MHz
    I2C_CCR = 80; // value for 100kHz I2C clock with 16MHz peripheral clock; APB1_CLK / (2 x I2C_FREQ) = 16,000,000 / 200,000 = 80
    I2C_TRISE = 17; // APB1_CLK1(MHz) + 1
    I2C_CR1 = 1; // last thing - ENABLE the peripheral
}

static uint8_t wait4flag(volatile uint32_t *reg, uint8_t bit, uint16_t timeout_ms) {
    uint16_t ms = 0;
    while(!(*reg & (1 << bit)) && ms < timeout_ms) {
        if(SYST_CSR & (1 << 16)) ms++;
    }
    return (ms < timeout_ms) ? 1 : 0; // 1 = didn't time out, 0 = timed out
}

uint8_t i2c_check_addresses(void) {

    usart_send_string("I2C addresses acknowledged: ");

    for(uint8_t i = 0; i <= 127; i++) {
        I2C_CR1 |= (1 << 8); // send start bit

        //wait until start bit sets to 1, meaning the info is on the bus, or ms get to 1000
        if(!(wait4flag(&I2C_SR1, 0, 1000))) {
            usart_send_string("Start bit timed out!\n\r");
            I2C_CR1 |= (1 << 9); // set end bit
            return 0;
        }                                  

        // 0 is the 0 position bit: 0 = write, 1 = read; in this case asking for I2C address 0
        I2C_DR = (i << 1) | 0;                      
        
        volatile uint16_t ms = 0;

        //wait until either address acknowledged or failure acknowledged, or ms reaches 1000
        while(!(I2C_SR1 & (1 << 1)) && !(I2C_SR1 & (1 << 10)) && ms < 1000) { // wait while these are waiting for flag
            if (SYST_CSR & (1 << 16)) {
                ms++;
            }
        }
         
        if(ms >= 1000) {
            I2C_CR1 |= (1 << 9); // set end bit
            usart_send_string("Ruh roh, address ");
            usart_send_num(i);
            usart_send_string(" didn't respond to the I2C scan.\n\r");
            return 0;
        }    

        uint8_t present = (I2C_SR1 & (1 << 1)) ? 1 : 0; // save whether address was acknowledged or failed 
        uint32_t temp = I2C_SR2; //have to read I2C_SR2 to clear ADDR flag
        (void) temp;

        if(present == 1) {
                usart_send_num(i);
                usart_send_string(", ");
            } else {
                I2C_SR1 &= ~(1 << 10); // have to manually clear AF ("acknoledge failure") if flag is set
            }
            
        // set stop flag (it's then automatically cleared by the hardware)
        I2C_CR1 |= (1 << 9); 
    }

    usart_send_string("\n\r");

    return 1;
}

uint8_t i2c_write (uint8_t addr, const uint8_t *data, uint16_t len) {
    // send start bit
    I2C_CR1 |= (1 << 8); 

     // wait until start bit sets to 1, meaning the info is on the bus
    if(!(wait4flag(&I2C_SR1, 0, 1000))) {
        usart_send_string("Oh no, I2C1's start bit was never sent!");
        // set stop flag
        I2C_CR1 |= (1 << 9); 
        return 0;
    }
    
    // put address and "write" bit in data register
    I2C_DR = (addr << 1) | 0; 

    if(!(wait4flag(&I2C_SR1, 1, 1000))) {
        //set stop bit
        I2C_CR1 |= (1 << 9); 
        usart_send_string("Oh no, the address I2C1 is trying to connect with wasn't acknowledged!");
        return 0;
    }

    uint32_t temp = I2C_SR2; // reading SR2 just to clear ADDR flag
    (void) temp;

    for(uint32_t i = 0; i < len; i++){
        // wait for TXE (transmit buffer empty) to be set in SR1
        if(!(wait4flag(&I2C_SR1, 7, 1000))) {
            I2C_CR1 |= (1 << 9); 
            usart_send_string("Oh no, the transmit buffer of I2C1 is stuck and not emptying!");
            return 0;
        }

        I2C_DR = data[i];
    }

    if(!(wait4flag(&I2C_SR1, 2, 1000))) {
        I2C_CR1 |= (1 << 9); 
        usart_send_string("Oh no, the byte transfer of I2C1 is stuck! The 'byte transfer finished' flag was never set.");
        return 0;
    }

    // set stop flag (it's then automatically cleared by the hardware)
    I2C_CR1 |= (1 << 9); 

    return 1;
}


