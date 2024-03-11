
#include "max7219.h"


namespace max7219 {


/**
Constructor.

Assign and configure MCU pins to use for driver.

Args:
    mosi: Data-out pin.
    clock: Clock-out pin.
    chip_select: Chip-select ('Load' on MAX7219) pin.
*/
MAX7219::MAX7219(uint8_t mosi, uint8_t clock, uint8_t chip_select) :
        mosi(mosi), clock(clock), chip_select(chip_select)  {
    // Data direction, set for output
    DDR |= (1<<mosi) | (1<<chip_select) | (1<<clock);

    // Set chip select high
    PORT |= (1<<chip_select);
}


/**
Put the chip into a useful state at start-up.

All digits are enabled, decode-mode is on, brightness is set, device is enabled.

Args:
    brightness (uint8_t): 0 to 15. See `set_brightness()`.
*/
void MAX7219::init(uint8_t brightness) {
    //set_test_mode(false);
    set_scan_limit(7);
    use_decode_mode(true);
    set_brightness(brightness);
    set_shutdown(false);
}


/**
Set brightness of entire display.

Changes the duty-cycle of the PWM used to run the display.

On both the MAX7219 and the MAX7221 there are 16 possible levels of brightness
to chose from.  On the MAX7219 they vary from 1/32 minimum to a max of 31/32. The
MAX7221 varies from 1/16 to 15/16.

Args:
    brightness (uint8_t): zero is dim, 15 is bright.

*/
void MAX7219::set_brightness(uint8_t brightness) {
    brightness = ( brightness > 15) ? 15 : brightness;
    transmit(0x0a, brightness);
}


/**
Enable chips BCD decode mode for use with 7-segment displays.

Args:
    do_decoding (bool): Use decode mode. Or don't. Whatevs.

When on, arguments to `set_digit()` are treated as BCD - ie. only the least-
significant four bits are considered, and they are treated as digits.

For example, calling ``set_digit(2, 7)`` would print '7' on a seven-segment display.

One oddity here is how is how the 'extra' states are used. The datasheet rather
enigmatically describes this as 'BCD Code B', which I've never heard of. If anybody
knows where this usage comes from, or where else it is used, I'd love to hear about
it.

Digits 0-9 are represented by bytes 0x00 to 0x09 as you'd expect. The weirdness comes
after that:

    0x0A    - (hyphen)
    0x0B    E
    0x0C    H
    0x0D    L
    0x0E    P
    0x0F    (blank)

I guess the ability to spell 'HELP' was more important to somebody than
printing hex!

(If you've made it this far you deserve to know that I have cut a corner
here by making the BCD decoding state boolean. It is the one piece of the
hardware I have not exposed completely. There are actually two intermediate
modes where decoding occurs only on groups of digits. See the datasheet for
what details there are, and send ``transmit(0x09, 0x01)`` or
``transmit(0x09, 0x0f)`` to try those modes).
*/
void MAX7219::use_decode_mode(bool do_decoding) {
    if ( do_decoding ) {
        transmit(0x09, 0xff);
    } else {
        transmit(0x09, 0x00);
    }
}


/**
Set the value of the given digit.

There are 8 digits, numbered 0 to 7. The data is interpreted as BCD digit if
`set_decode_mode(true)` a raw bit-pattern otherwise.

Args:
    digit (uint8_t): Digit to set, 0 to 7
    data (uint8_t): Value to use for given digit.

*/
void MAX7219::set_digit(uint8_t digit, uint8_t data) {
    digit = ( digit > 7 ) ? 7 : digit;
    transmit(digit + 1, data);
}


/**
Number of digits to enable.

The datasheet contains an adonishment not to use this feature to blank
leading zeros; changing the scan limit also changes the muliplexing frequency
*and* the current per segment.
*/
void MAX7219::set_scan_limit(uint8_t limit) {
    limit = ( limit < 1 ) ? 1 : limit;
    limit = ( limit > 8 ) ? 8 : limit;
    transmit(0x0b, (limit-1));
}


/**
Bring the chip in or out of 'shutdown' mode.

That is the term in the dataheet, but it's a bit of a misnomer, and all commands
are still accepted. It's more like a 'blank-display' mode, and is useful for
flashing the display, or saving power by powering down the LEDs temporarily.

Args:
    do_shutdown (bool): Set true to blank display.
*/
void MAX7219::set_shutdown(bool do_shutdown) {
    if( do_shutdown ) {
        transmit(0x0c, 0x00);
    } else {
        transmit(0x0c, 0x01);
    }
}


/**
Turn test mode on or off.

All LEDs are lit when test mode is on.

Args:
    do_led_test (bool): Should test mode be on or off?
*/
void MAX7219::set_test_mode(bool do_led_test) {
    if( do_led_test ) {
        transmit(0x0f, 0x01);
    } else {
        transmit(0x0f, 0x00);
    }
}


/**
Send low-level command to chip.

See the data sheet for valid addresses and data bytes.

Args:
    address (uint8_t): Register address.
    data (uint8_t): Data to send.
*/
void MAX7219::transmit(const uint8_t& address, const uint8_t& body) {
    uint8_t buffer[2];
    buffer[0] = address;
    buffer[1] = body;

    // CS Low
    bit_clear(PORT, chip_select);

    // Send
    for (uint8_t i=0; i<2; ++i) {
        uint8_t byte = buffer[i];
        for (uint8_t bit=7; bit<255; --bit) {
            bit_clear(PORT, clock);
            bit_write(PORT, mosi, bit_get(byte, bit));
            bit_set(PORT, clock);
        }
    }

    // CS High
    bit_set(PORT, chip_select);
}


} // namespace max7219
