/**
Playing around with a bunch of LEDs directly driven off a ATMega168.
*/


#include <avr/io.h>
#include <math.h>
#include <stdlib.h>
#include <util/delay.h>


#define DELAY       25
#define prime_t     uint32_t
#define bool        uint8_t


/**
Enough data to initialise and display single LED.
*/
typedef struct led_pin {
    volatile uint8_t *port;     /** Address of pin's port, eg. &PORTB */
    volatile uint8_t *ddr;      /** Address of pin's DDR, eg. &DDRD */
    uint8_t pin;                /** Pin's number, eg. 0 */
} led_pin;


/**
A string of leds, in order of display.
*/

#define num_leds 18

led_pin leds[num_leds] = {
    {&PORTB, &DDRB, 4},
    {&PORTB, &DDRB, 3},
    {&PORTB, &DDRB, 2},
    {&PORTD, &DDRD, 2},
    {&PORTD, &DDRD, 3},
    {&PORTD, &DDRD, 4},
    {&PORTB, &DDRB, 6},
    {&PORTC, &DDRC, 5},
    {&PORTC, &DDRC, 4},
    {&PORTC, &DDRC, 3},
    {&PORTC, &DDRC, 2},
    {&PORTC, &DDRC, 1},
    {&PORTC, &DDRC, 0},
    {&PORTB, &DDRB, 7},
    {&PORTD, &DDRD, 5},
    {&PORTD, &DDRD, 6},
    {&PORTD, &DDRD, 7},
    {&PORTB, &DDRB, 0},
};


/**
Turn on just the LED at the given index.

@param number Index into global leds array.
*/
void led_on(uint8_t number) {
    volatile uint8_t *port = leds[number].port;
    uint8_t pin = leds[number].pin;
    *port |= (1<<pin);
}

/**
Turn on all LEDs in string.
*/
void led_on_all() {
    for(uint8_t i=0; i<num_leds; i++) {
        led_on(i);
    }
}


/**
Turn off just the LED at the given index.

@param number Index into global leds array.
*/
void led_off(uint8_t number) {
    volatile uint8_t *port = leds[number].port;
    uint8_t pin = leds[number].pin;
    *port &= ~(1<<pin);
}


/**
Turn off all the LEDs in the string.
*/
void led_off_all() {
    for(uint8_t i=0; i<num_leds; i++) {
        led_off(i);
    }
}


/**
Print the given integer in binary out the LED string.

Uses the first LED in the string as the least significant bit.
*/
void print_binary(uint32_t number) {
    uint8_t bit;
    for(uint8_t i=0; i<num_leds; i++) {
        bit = (number >> i) & 1;
        if (bit) {
            led_on(i);
        } else {
            led_off(i);
        }
    }
}


/**
Quick and (very) dirty primality test.
*/
inline uint8_t is_prime(prime_t n) {
    prime_t i;
    for (i=3; i<n; i+=2) {
        if (n % i == 0) {
            return 0;
        }
    }
    return 1;
}


/**
a prime (except 2 and 3) is of form 6k-1 and 6k+1 and looks only at
divisors of this form.
*/
inline bool is_prime2(prime_t n) {
    if ((n == 2) || (n == 3)) {
        return 1;
    }

    if ((n % 2 == 0) || (n % 3 == 0)) {
        return 0;
    }

    prime_t i = 5;
    prime_t w = 2;
    while ((i * i) <= n) {
        if (n % i == 0) {
            return 0;
        }
        i += w;
        w = 6 - w;
    };
    return 1;
}


/**
Print, in binary, all the primes possible given length of LED string.
*/
void primes() {
    prime_t limit = (prime_t) pow(2, num_leds);
    for (prime_t i=2; i<limit; i++) {
        if (is_prime2(i)) {
            print_binary(i);
            //_delay_ms(DELAY);
        }
    }
}


/**
Count in binary up to the largest number possible with available LEDs.
*/
inline void count() {
    uint32_t limit = (uint32_t) pow(2, num_leds);
    for (uint32_t i=1; i<limit; i++) {
        print_binary(i);
        //_delay_ms(DELAY);
        led_off_all();
    }
}


/**
Show a 'Cylon' or 'Knight-Rider' light effect.

The first and last LEDs in string are show for twice as long so as to
equalise their average brightness with the middle LEDs (which are lit twice
as often per cycle).
*/
void cylon() {
    // All LEDs in order
    for (uint8_t i=0; i<num_leds; i++) {
        led_on(i);
        _delay_ms(DELAY);
        // Double delay for first and last LEDs
        if ((i==0) | (i==(num_leds-1))) {
            _delay_ms(DELAY);
        }
        led_off(i);
    }

    // Middle LEDs only, in reverse order
    for (uint8_t i=(num_leds-2); i>0; i--) {
        led_on(i);
        _delay_ms(DELAY);
        led_off(i);
    }
}


/**
Prepare all LED pins in string for output.
*/
void setup() {
    volatile uint8_t *ddr;
    uint8_t pin;
    for (uint8_t i=0; i<num_leds; i++) {
        ddr = leds[i].ddr;
        pin = leds[i].pin;
        *(ddr) |= (1<<pin);
    }
}


int main() {
    setup();
    while (1) {
        cylon();
    }
    return 0;
}
