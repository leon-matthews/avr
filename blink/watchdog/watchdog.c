/*
Blink an LED using watchdog interupts, sleeping MCU as much (and as deeply) as possible.
*/

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <avr/wdt.h>
#include <stdbool.h>
#include <util/delay.h>

#include "pins.h"
#include "watchdog.h"


// Times (see <avr/wdt.h> for possible values)
#define OFF_TIME WDTO_1S
#define ON_TIME WDTO_60MS


volatile bool should_blink_led;


ISR(WDT_vect) {
    should_blink_led =  !should_blink_led;
}


void turn_led_on() {
    BLINK_LED_PORT = (1 << BLINK_LED);
    change_watchdog_interrupt(ON_TIME);

}

void turn_led_off() {
    BLINK_LED_PORT &= ~(1 << BLINK_LED);
    change_watchdog_interrupt(OFF_TIME);
}


void setup() {
    setup_watchdog_as_interrupt(ON_TIME);
    BLINK_LED_DDR |= (1 << BLINK_LED);
    // Start with LED on
    should_blink_led = true;
}


void sleep() {
    set_sleep_mode(SLEEP_MODE_PWR_DOWN);
    sleep_mode();
}


void main() {
    setup();
    while(true) {
        if(should_blink_led) {
            turn_led_on();
        } else {
            turn_led_off();
        }
        sleep();
    }
}
