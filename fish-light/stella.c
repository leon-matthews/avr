/**
Stella's totally awesome fish lamp! It is designed to display
the fish as well as perform as a night light.

Lamp has two LED strips (one red and one white), a single
on/off button, and a light sensor. Each of the LED strips is
PWM driven to allow for smooth fades in and out.

The lamp will glow white if on in a bright room, but red if the
room is dark so as to act as a night light.

When red the lamp will very gradually fade down to off.
*/


#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/power.h>
#include <avr/sleep.h>
#include <stdint.h>
#include <stdbool.h>
#include <util/delay.h>


#define RED_PIN     PINB0
#define WHITE_PIN   PINB1


enum states {
    FADE_IN_RED,        // Bring red LEDs up, before FADE_OUT_WHITE.
    FADE_IN_WHITE,      // Bring white LEDs up quickly, before FADE_OUT_RED.
    FADE_OUT_RED,       // Extinguish red LEDs on the way to WHITE_ON.
    FADE_OUT_WHITE,     // Extinguish white LEDs on the way to RED_ON
    INIT,               // Start state
    OFF,                // ALl LEDs off. Sleep until button pressed.
    RED_ON,             // Red LEDs on, fading very slowly to black.
    WHITE_ON,           // White LEDs on, waiting for room to go dark.
};


// Forward declations
bool is_button_pressed();
bool is_room_dark();
void timer0_init();


// Buffer brightness values
volatile uint8_t red_pwm = 0;
volatile uint8_t white_pwm = 0;
enum states state = INIT;
uint8_t i;


void setup() {
    // Start with PWM disabled
    DDRB &= ~(1 << RED_PIN);
    DDRB &= ~(1 << WHITE_PIN);

    // Start PWM on pins OC0A (PB0) and OC1A (PB1)
    timer0_init();
}


void main() {
    setup();

    while (true) {

        switch(state) {
            case FADE_IN_RED:
                /**
                Fade red REDs up to full.
                */
                // TODO Setup interupt timer speed and brightness deltas
                if (red_pwm == 255) {
                    state = FADE_OUT_WHITE;
                } else {
                    red_pwm++;
                    _delay_ms(39);          // 10 seconds max
                }
                break;

            case FADE_IN_WHITE:
                /**
                Fade white LEDs up to full on the way to WHITE_ON.
                */
                if (white_pwm == 255) {
                    state = FADE_OUT_RED;
                } else {
                    white_pwm++;
                    _delay_ms(12);          // 3 seconds max
                }
                break;

            case FADE_OUT_RED:
                /**
                Fade red out quickly on the way to WHITE_ON.
                */
                if (red_pwm == 0) {
                    DDRB &= ~(1 << RED_PIN);
                    state = WHITE_ON;
                } else {
                     red_pwm--;
                    _delay_ms(12);          // 3 seconds max
                }
                break;

            case FADE_OUT_WHITE:
                /**
                Fade white out to off.
                */
                if (white_pwm == 0) {
                    DDRB &= ~(1<<WHITE_PIN);
                    state = RED_ON;
                } else {
                     white_pwm--;
                    _delay_ms(118);         // 30 seconds max
                }
                break;

            case INIT:
                /**

                */
                if (is_room_dark()) {
                    state = RED_ON;
                } else {
                    state = WHITE_ON;
                }
                break;

            case OFF:
                /**
                Everything off. Wait for button press.
                */
                if (is_button_pressed()) {
                    state = FADE_IN_WHITE;
                }
                break;

            case RED_ON:
                /**
                Red on full. Fade slowly to off.
                */
                if (red_pwm == 0) {
                    DDRB &= ~(1 << RED_PIN);
                    state = OFF;
                } else {
                     red_pwm--;

                    _delay_ms(1000);        // 255 seconds max
                }
                break;

            case WHITE_ON:
                /**
                White LEDs on full.
                Wait for room to go dark.
                */
                if (is_room_dark()) {
                    state = FADE_IN_RED;
                }
                break;
        }
    }

    // Toggle ports to allow PWM ports to go fully off.
    if (red_pwm == 0) {
        DDRB &= ~(1 << RED_PIN);
    } else {
        DDRB |= (1 << RED_PIN);
    }

    if (white_pwm == 0) {
        DDRB &= ~(1 << WHITE_PIN);
    } else {
        DDRB |= (1 << WHITE_PIN);
    }
}


bool is_room_dark() {
    return false;
}


bool is_button_pressed() {
    return true;
}


/**
Setup timer0 operation.

1) Fast PWM for variable brightness on red and white
   MOSFET driver pins

2) Enable overflow interupt handler that will update
   the PWM duty only at the start of a new cycle.
*/
void timer0_init() {
    cli();

    // TCCR Timer/Counter Control Register
    // Prescaler 1/8, 245Hz.
    TCCR0B |= (1<<CS01);
    // Fast PWM
    TCCR0A |= (1<<WGM01) | (1<<WGM00);
    // Non-inverting output on pins OC0A (PB0) and OC1A (PB1)
    TCCR0A |= (1<<COM0A1) | (1<<COM0B1);

    // TIMSK - Timer/Counter Interrupt Mask Register
    // Interupt on overflow
    TIMSK |= (1<<TOIE0);
    sei();
}


/**
Timer0 ISR: Counter overflow

Update PWM 'brightness' values from buffers on counter overflow.
*/
ISR(TIM0_OVF_vect) {
    // OCR - Output Control Registers
    OCR0A = red_pwm;
    OCR0B = white_pwm;
}
