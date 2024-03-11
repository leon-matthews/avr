
#include <stdlib.h>
#include <util/delay.h>

#include "max7219.h"


auto display = max7219::MAX7219(PB2, PB3, PB4);


void random_digit() {
    uint16_t rand = random();
    uint8_t digit = (rand & 0x00FF) >> 5;   // Top three bits of LSB
    uint8_t value = rand % 10;
    display.set_digit(digit, value);
}


void random_pattern() {
    uint16_t rand = random();
    uint8_t digit = (rand & 0x00FF) >> 5;   // Top three bits of LSB
    uint8_t value = (rand >> 10);
    display.set_digit(digit, value);
}


void main() {
    display.init();
    display.use_decode_mode(false);
    display.set_scan_limit(8);

    uint16_t count = 0;
    while (true) {
        random_pattern();
        _delay_ms(100);

        ++count;
        if ( ! (count % 10) ) {
            display.set_shutdown(false);
        } else {
            display.set_shutdown(true);
        }
    }
}
