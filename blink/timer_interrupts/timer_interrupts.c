/*
Blink an LED using timer2 interupt and a one millisecond update tick.
*/

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/power.h>
#include <avr/sleep.h>
#include <stdbool.h>
#include <stdint.h>
#include <util/delay.h>

#include "pins.h"


// Times
// Maximum 65,535 milliseconds (~1 minute)
#define LED_OFF_TIME 950
#define LED_ON_TIME 50


void init_timer2();

volatile bool led_on = false;
volatile uint16_t led_elapsed = 0;


void setup() {
    // Enable LED pin, turn on LED
    BLINK_LED_DDR |= (1 << BLINK_LED);
    BLINK_LED_PORT |= (1 << BLINK_LED);
    led_on = true;

    // Boost CPU frequency to 8MHz
    clock_prescale_set(clock_div_1);

    // Enable heartbeat output
    HEARTBEAT_DDR |= (1 << HEARTBEAT);

    // Start 1ms interupts
    init_timer2();
}


/**
Update LED once a tick.
*/
void update_led()
{
    led_elapsed++;
    if(led_on) {
        if(led_elapsed > LED_ON_TIME) {
            // Turn off
            BLINK_LED_PORT &= ~(1 << BLINK_LED);
            led_elapsed = 0;
            led_on = false;
        }
    } else {
        if(led_elapsed > LED_OFF_TIME) {
            // Turn on
            BLINK_LED_PORT |= (1 << BLINK_LED);
            led_elapsed = 0;
            led_on = true;
        }
    }
}


/**
System tick.

The HEARTBEAT pin is set to high while tasks are running, then is turned off
again until the next tick. The frequency of this output can be used to verify
tick frequency, and interestingly, its duty-cycle indicates CPU load.
*/
void inline tick() {
    HEARTBEAT_PORT |= (1 << HEARTBEAT);
    update_led();
    HEARTBEAT_PORT &= ~(1 << HEARTBEAT);
}


void main() {
    setup();
    set_sleep_mode(SLEEP_MODE_IDLE);
    while(true) {
        sleep_mode();
    }
}


/**
Setup timer2 to provide 1000Hz interupts.
*/
void init_timer2() {
    cli(); // stop interrupts
    TCCR2A |= (1 << WGM21);
    TCCR2B |= (1 << CS21) | (1 << CS20);    // Prescaler = 64x
    OCR2A = 249;                            // = F_CPU/(prescaler * 1000ms) - 1
    TIMSK2 |= (1 << OCIE2A);
    sei(); // allow interrupts
}


/**
Timer2 interrupt service routine.
*/
ISR(TIMER2_COMPA_vect){
    tick();
}
