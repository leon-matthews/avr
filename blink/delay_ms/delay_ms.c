/*
Blink an LED using `_delay_ms()`.
*/

#include <assert.h>
#include <avr/io.h>
#include <stdbool.h>
#include <util/delay.h>

#include "pins.h"


// Times (in milliseconds)
#define LOOP_TIME 1000
#define ON_TIME 50


// Compile-time error checking
#define OFF_TIME (LOOP_TIME - ON_TIME)
static_assert(OFF_TIME > 0, "ON_TIME must be less than LOOP_TIME");


void setup() {
    BLINK_LED_DDR |= (1 << BLINK_LED);
}


void main() {
    setup();
    while(true) {
        // Set
        BLINK_LED_PORT ^= (1 << BLINK_LED);
        _delay_ms(ON_TIME);

        // Clear
        BLINK_LED_PORT &= ~(1 << BLINK_LED);
        _delay_ms(OFF_TIME);
    }
}
