#pragma once

#include <stdint.h>
#include <avr/io.h>

#include "../common.h"


#define DDR     DDRB
#define PORT    PORTB


namespace max7219 {


/**
Driver for the MAX7219 and MAX7221 LED driver chip.

It multiplexes up to 64 LEDs in 8 groups of 8. It can painlessly drive eight digits
of a 7-segment display (never forgetting a decimal point for every digit), or a 8x8
LED matrix using only three pins from your micro.

This driver allows use of any three spare bins by 'bit-banging' the communication. The
communication is mostly SPI - but the MAX7219 does not implement chip select in the
usual way. The approach is a good fit as the chip doesn't need the bandwidth of
hardware SPI, freeing up that MCU feature for another device.

Even high refresh rates on a 8x8 LED matrix is undemanding.

A average clock of only 5kHz is required to send 10 full updates per second: 16 bits
for address plus body, times eight digits, times 10 updates = 5120Hz. Note that new
data need only be sent when a digit actually changes; the chip handles display
muliplexing with a frequency greater than 500Hz.

Were you to chain 16 chips together and run an animation at 60Hz the clock would need to
run at an average of 500kHz. You may want to switch to hardware SPI in this case...

Provides a fairly direct interface to the chips functionality. Abstractions
are left for higher levels of code.
*/
class MAX7219 {
    private:
        const uint8_t mosi;
        const uint8_t clock;
        const uint8_t chip_select;

    public:
        MAX7219(uint8_t data, uint8_t clock, uint8_t chip_select);
        void init(uint8_t brightness=8);
        void set_brightness(uint8_t brightness);
        void use_decode_mode(bool do_decoding);
        void set_digit(uint8_t digit, uint8_t data);
        void set_scan_limit(uint8_t digits);
        void set_shutdown(bool);
        void set_test_mode(bool);
        void transmit(const uint8_t& address, const uint8_t& body);
};


} // max7219
