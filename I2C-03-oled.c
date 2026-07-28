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

void usart_init(void) {
    GPIOA_MODER &= ~(15 << 4); 
    GPIOA_MODER |= (2 << 4) | (2 << 6); // set PA2 and PA3 to alternate function 

    GPIOA_AFRL &= ~((15 << 8) | (15 << 12));
    GPIOA_AFRL |= (7 << 8) | (7 << 12); // set PA2 and P3 to alt func 7, which is USART2

    USART_BRR = (8 << 4) | 11; //that crazy baud rate mantissa/fraction setup

    NVIC_ISER1 |= (1 << 6); //bit 6 on ISER1 for interrupt 38 (each ISER bus carries 32 interrupts)
    
    USART_CR1 |= (1 << 3) | (1 << 2) | (1 << 5) | (1 << 13); //TX enable, RX enable, RX interrupt enable, UART unit enable
}

void systick_init(void) {
    SYST_RVR = 15999; // value = N - 1, where N is number of system clock pulses (16MHz)
    SYST_CVR = 0; // set current (starting) value, to be safe in case for some reason starts at non-zero
    SYST_CSR |= 1 | (1 << 2); // enables systick clock and sets clock to internal ("processor clock")
}

void usart_send_char(char c) {
    while(!(USART_SR & (1 << 7))); // wait until buffer is empty
    USART_DR = c; // fill buffer with char
}

void usart_send_string(char* str){
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

uint8_t wait4flag(volatile uint32_t *reg, uint8_t bit, uint16_t timeout_ms) {
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

uint8_t i2c_write (uint8_t addr, const uint8_t *data, uint8_t len) {
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

const uint8_t oled_init_seq[] = {
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
    0xD5, 0x80, // define divide ratio of internal clock (CLK) and display divider (D)
    0x8D, 0x14, // enable charge pump regulator
    0xAF // display ON
};

void ssd1306_oled_init(void) {
    uint8_t len = sizeof(oled_init_seq) / sizeof(oled_init_seq[0]);
    i2c_write(0x3C, oled_init_seq, len);
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
    i2c_check_addresses();
    ssd1306_oled_init();

    uint8_t screen_on[] = {0x00, 0xA5};
    i2c_write(0x3C, screen_on, 2);
    delay_ms(5000);

    uint8_t screen_off[] = {0x00, 0xA4};
    i2c_write(0x3C, screen_off, 2);

    while(1) {}
}
