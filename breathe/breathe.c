/*
PWM example.
*/


#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <avr/sleep.h>
#include <stdint.h>
#include <stdbool.h>
#include <util/delay.h>

#include "pins.h"


/**
Use timer 0 to power LED, with variable intensity, using fast PWM.
*/
void init_timer0A()
{
    // Timer/Counter Control Registers A & B (TCCR0A & TCCR0B)

    // Compare output mode (COM)
    // (Duty-cycle configured using Output Compare Register (OCR0A)
    TCCR0A |= (1 << COM0A1);                // Clear OC0A on compare match (non-inverting)

    // Waveform generation mode (WGM)
    TCCR0A |= (1 << WGM00) | (1 << WGM01);  // Fast PWM

    // Clock select (CS)
    // Frequency equals F_CPU / 256 / prescaler
    // eg. 1e6 / 256 / 1 = 3906Hz
    TCCR0B |= (1 << CS00);                  // Set prescaler to 1
}


/**
Use timer 2 to change duty cycle of LED PWM, via its ISR.
*/
void init_timer2()
{
    // Waveform generation mode (WGM)
    TCCR2A |= (1 << WGM21);         // CTC mode, zero to OCR2A
    OCR2A = 120;                    // = F_CPU/(prescaler * 1000ms) - 1

    // Clock select (CS)
    TCCR2B |= (1 << CS22) | (0 << CS21) | (1 << CS20);    // 128

    // Timer Interupt MaSK (TIMSK2)
    // Output Counter Interrupt Enable (OCIE)
    TIMSK2 |= (1 << OCIE2A);
}


/**
Timer 2 interrupt service routine.
*/

extern const uint8_t brightnesses[] PROGMEM;
extern const uint8_t num_brightnesses;
uint8_t brightness_index = 0;

ISR(TIMER2_COMPA_vect){
    OCR0A = brightnesses[brightness_index];
    if (brightness_index == num_brightnesses-1) {
        brightness_index = 0;
    } else {
        brightness_index++;
    }
}


void setup()
{
    BLINK_LED_DDR |= (1 << BLINK_LED);
    init_timer0A();
    init_timer2();
    sei();
}


void main() {
    setup();
    while(true) {
    }
}