#ifndef I2C_H
#define I2C_H

#include <stdint.h>

void i2c_init(void);
uint8_t i2c_check_addresses(void);
uint8_t i2c_write (uint8_t addr, const uint8_t *data, uint16_t len);

#endif
