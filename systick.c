#include <stdint.h>
#include "registers.h"
#include "systick.h"

void systick_init(void) {
    SYST_RVR = 15999; // value = N - 1, where N is number of system clock pulses (16MHz)
    SYST_CVR = 0; // set current (starting) value, to be safe in case for some reason starts at non-zero
    SYST_CSR |= 1 | (1 << 2); // enables systick clock and sets clock to internal ("processor clock")
}

void delay_ms(uint16_t ms) {
    for (uint16_t i = 0; i < ms; i++) {
        while(!(SYST_CSR & (1 << 16)));
    }
}
