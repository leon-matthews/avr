/*
Blink an LED using timer0 to drive pin OC0A (D6) directly.
*/

#include <avr/sleep.h>
#include <stdbool.h>

#include "pins.h"


void init_timer0() {
    TCCR0A |= (1 << WGM01);             // CTC mode
    TCCR0A |= (1 << COM0A0);            // Toggle pin
    TCCR0B |= (1 << CS00) | (1 <<CS02); // 8x prescaler
    OCR0A = 255;                        // 500ms
}


void setup() {
    BLINK_LED_DDR |= (1 << BLINK_LED);
    BLINK_LED_PORT |= (1 << BLINK_LED);
    init_timer0();
}


void main() {
    setup();
    set_sleep_mode(SLEEP_MODE_IDLE);
    while(true) {
        sleep_mode();
    }
}
