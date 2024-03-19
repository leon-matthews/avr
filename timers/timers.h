#pragma once

#include <avr/io.h>


/**
A tiny AVR timer library, just to cover common-use cases.

It's structured as stand-alone functions, to allow for minimal code size, and to mix well
with manual overriding of the many various buttons and knobs available.  This
documentation is arguably the most important part - I find the register names difficult
to differentiate, so it's nice to have them spelled out. Even if I'm the one doing the
spelling.

Speaking of confusingly named registers, here's a brief overview:

``TCNTn``
    Timer/Counter register. One per timer, ie. there are ``TCNT0``, ``TCNT1``, and
    ``TCNT2``. Incremented by the MCU from a variety of sources, at a variety of speeds.

``OCRnA``
    Output Compare Register. There are two per timer, eg. ``OCR0A`` & ``OCR0A``. Their
    value is always compared their ``TCNTn`` register. Various actions can occur when
    they match. Try your best not to confuse them with the timer output pins ``OCnA``
    and ``OCnB`` (Output Compare Match).

``TCCRnA`` & ``TCCRnB``
    Timer/Counter Control Register.  Configuration central. Two bytes worth.
    Lots and lots of possible combinations... Confusingly, most of the bits *inside*
    these registers also have their own names. eg. ``COMnA``, ``WGMnn``, ``CSnn``.

*/
namespace timers {


enum class Clock : byte {
    Stopped         = 0x00,
    Divide_by_1     = (1 << CS20),
    Divide_by_8     = (1 << CS21),
    Divide_by_32    = ((1 << CS20) | (1 << CS21)),
    Divide_by_64    = (1 << CS22),
    Divide_by_128   = ((1 << CS20) | (1 << CS22)),
    Divide_by_256   = ((1 << CS21) | (1 << CS22)),
    Divide_by_1024  = ((1 << CS20) | (1 << CS21) | (1 << CS22)),
};


/**
TIMER0: An 8-bit timer.

In the Arduino ecosystem `timer0` is already setup and running. They
use the overflow ISR to update the tick count for the `millis()` and `micros()`
functions.

You can still use the two output compare channels, but you should leave
the rest of the timer alone if you want Arduino code and libraries to work.
*/
namespace timer0 {
} // namespace timer0


/**
TIMER1: A 16-bit timer.
*/
namespace timer1 {
} // namespace timer1


/**
TIMER2: a 8-bit timer.

Two output pins and three ISR can be enabled at various frequencies.
*/
namespace timer2 {
    /**
    There are three possible interupts for timer2.
    */
    namespace interupts {
        constexpr byte overflow     = (1 << TOIE2);
        constexpr byte match_OCR2A  = (1 << OCIE2A);
        constexpr byte match_OCR2B  = (1 << OCIE2B);
    }

    /**
    Possible values for output on pin OC2A.

    (ATMega328 pin PB3, or Arduino pin 11).

    Use `disconnected` to allow usage of the pin elsewhere.

    In normal and CTC modes `toggle` is usually the right choice [1].

    In the PWM modes choose from `inverting` and `non_inverting` as you like, noting
    the asymmetry in the end-points:

    `non_inverting`
        PWM duty-cycle goes from 1% to 100% as OCR2A goes from 0 to 255.

    `inverting`
        PWM duty-cycle goes from 99% to 0% as OCR2A goes from 0 to 255.

    [1] `inverting` is actually ``set`` from the datasheet. It will set OC2A on match.
        `non_inverting` is ``clear``. It will clear OC2A on compare match. I guess that
        could be useful, maybe. Sometime, somewhere...
    */
    enum class output_a : byte {
        disconnected    = 0x00,
        toggle          = (1 << COM2A0),
        non_inverting   = (1 << COM2A1),
        inverting       = ((1 << COM2A0) | (1 << COM2A1)),
    };

    /**
    Possible values for output on pin OC2AB.

    This is AVR pin PD3, or Arduino pin 3.
    */
    enum class output_b : byte {
        disconnected    = 0x00,
        toggle          = (1 << COM2B0),
        non_inverting   = (1 << COM2B1),
        inverting       = ((1 << COM2B0) | (1 << COM2B1)),
    };


    /**
    Set timer2 to run in `Clear Timer on Compare (CTC)` mode.

    This is like `normal` mode, except that the frequency is far more readily
    adjustable, from a maximum of `F_CPU/2` (eg. 8MHz on Arduino Uno) all the way down
    to `F_CPU/(2 * 1024 * (255 + 1))` (30.5Hz).

    Rather than ``TCNT2`` counting all the way from 0 to 255, the value of ``OCR2A`` is
    used as the counter's TOP instead. Additionally, the counter register is
    automatically zeroed when the top is reached.

    Frequency is set by ``OCR2A``, lower values for higher frequencies:

        frequency = F_CPU / 2 * prescaler * (1 + OCR2A)

    For example, on an Arduino running at 16MHz we range (using the fastest
    prescaler) from 31kHz to 8MHz:

        lowest = 16MHz / 2 * 1 * (1 + 255) = 16MHz / 512 = 31.25kHz
        highest = 16MHz / 2 * 1 * (1 + 0) = 16MHz / 2 = 8MHz

    For example, on the Uno we can roughly sweep a large chunk of the
    audible spectrum - from 30Hz up 7.8kHz::

        set_mode_ctc();
        use_prescaler(Clock::Divide_by_1024);
        use_outputs(output_a::toggle);

        uint8_t i = 0;
        while(true) {
            // Count up to 255...
            do {
                OCR2A = i;
                _delay_ms(10);
            } while (i++ != 255);
            // (Be careful, ``i`` has wrapped around to zero here!)

            // ...and back down to zero
            i = 255;
            do {
                OCR2A = i;
                _delay_ms(10);
            } while (i--);
        }

    */
    void set_mode_ctc() {
        TCCR2A &= ~(1 << WGM20);
        TCCR2A |= (1 << WGM21);
        TCCR2B &= ~(1 << WGM22);
    }


    /**
    Set timer2 to run in the not-super-useful 'Normal' mode.

    Simply has ``TCNT2`` run from zero to 255 over and over again, so the frequency
    is only adjustable using the prescaler.

    Output frequency is given by::

        F_CPU / Prescaler / (TOP + 1)

        eg. TOP = 255 for 8-bit timers, so, 16MHz / 1024 / (255+1) = 30.5Hz

    Most useful for manually timing some event, as long as you're careful about
    avoiding overflow. Easier with a 16-bit counter::

        set_mode_normal();
        use_prescaler(Clock::Divide_by_8);  // F_CPU/8, 1MHz say.
        TCNT2 = 0;                          // Reset counter
        do_something();
        elapsed = TCNT2;

    Interestingly, when toggling outputs A and B their frequency and duty cycle
    are fixed, but their relative phase can be using using different
    values for ``OCR2A`` and ``OCR2B``. Inverted outputs are handy::

        use_outputs(output_a::toggle, output_b::toggle);
        OCR2A = 0;
        OCR2B = 127;
    */
    void set_mode_normal() {
        TCCR2A &= ~((1 << WGM20) | (1 << WGM21));
        TCCR2B &= ~(1 << WGM22);
    }


    /**
    Two independent (and fast) PWM outputs, using only fixed frequencies.

    Pulse width modulation using one of the frequencies given by:

        `F_CPU / prescaler * (256)`.

    Use OCR2A to set duty-cycle for output A, OCR2B for output B::

        set_mode_pwm_fast();
        use_prescaler(Clock::Divide_by_128);    // 485Hz on Arduino Uno
        use_outputs(output_a::non_inverting, output_b::non_inverting);
        OCR2A = 0;
        OCR2B = 255;

    */
    void set_mode_pwm_fast() {
        TCCR2A |= ((1 << WGM20) | (1 << WGM21));
        TCCR2B &= ~(1 << WGM22);
    }


    /**
    Use timer2 in 'Fast PWM', mode 7.

    Enables much higher frequency PWM, at the cost of having but a single output
    running at reduced resolution.

    Set frequency with OCR2A, duty-cycle with OCR2B. PWM output appears on OC2B::

        set_mode_pwm_faster();
        OCR2A = 10;
        OCR2B = 3;
        use_prescaler(Clock::Divide_by_64);
        use_outputs(output_a::disconnected, output_b::non_inverting);

    Frequency = F_CPU / prescaler / OCR2A

    Like 'Fast PWM', but counts from 0->OCR2A rather than 0->255. Lower
    values of OCR2A give higher frequencies.

    OCR2B is used to set the duty cycle. If it is higher than OCR2A there will
    be no output.

    One-shot mode
    =============

    1) Set OCR2A = 0
    2) Set OCR2B for width of pulse, in clock cycles: OCR2B = 255 - (cycles-1)
    3) Fire pulse by setting TCNT2 = OCR2B - 1

    For example, for a one-cycle pulse every millisecond.

        set_mode_pwm_faster();
        use_outputs(output_a::disconnected, output_b::inverting);
        OCR2A = 0;
        OCR2B = 255;    // 255 - (1-1)
        use_prescaler(Clock::Divide_by_1);
        while( true ) {
            _delay_ms(1);
            TCNT2 = 254;    // 255 - 1
        }

    Credit for the technique goes to:
    https://wp.josh.com/2015/03/12/avr-timer-based-one-shot-explained/
    */
    void set_mode_pwm_faster() {
        TCCR2A |= ((1 << WGM20) | (1 << WGM21));
        TCCR2B |= (1 << WGM22);
    }


    /**
    Use timer2 in 'Phase correct PWM' mode.

    Like 'Fast PWM' mode, but without phase-change glitches when duty-cycle changes - at
    the price of half the frequency::

        `F_CPU / 2 * prescaler * (256)`

    Use OCR2A to set duty-cycle for output A, OCR2B for output B::

        set_mode_pwm_fast();
        use_prescaler(Clock::Divide_by_128);    // 244Hz on Arduino Uno
        use_outputs(output_a::non_inverting, output_b::non_inverting);
        OCR2A = 0;
        OCR2B = 255

    */
    void set_mode_pwm_phase_correct() {
        TCCR2A |= (1 << WGM20);
        TCCR2A &= ~(1 << WGM21);
        TCCR2B &= ~(1 << WGM22);
    }


    void use_outputs(output_a a, output_b b=output_b::disconnected) {
        TCCR2A &= ~((1 << COM2A0) | (1 << COM2A1));
        TCCR2A |= static_cast<byte>(a);
        TCCR2A &= ~((1 << COM2B0) | (1 << COM2B1));
        TCCR2A |= static_cast<byte>(b);
    }

    /**
    Enable (or disable) interupt handlers.

    See the values of `timer2::interupts`. Interupt handlers should be as short as
    possible.

        ISR(TIMER2_OVF_vect) {
            // TCNT2 overflow
        }

        ISR(TIMER2_COMPA_vect) {
            // TCNT2 matches OCR2A
        }

        ISR(TIMER2_COMPB_vect) {
            // TCNT2 matches OCR2A
        }

    Ensure that ``sei()`` is called at the appropriate moment, too.

    Args:
        value (byte):
            Bits to enable. eg::

                use_interupts(interupts::overflow | interupts::match_OCR2A)

    */
    void use_interupts(byte value) {
        TIMSK2 &= ~((1 << TOIE2) | (1 << OCIE2A) | (1 << OCIE2B));
        TIMSK2 |= value;
    }


    /**
    Start the clock on timer2 and set its prescaler.
    */
    void use_prescaler(Clock prescaler) {
        TCCR2B &= ~( (1 << CS20) | (1 << CS21) | (1 << CS22) );
        TCCR2B |= static_cast<byte>(prescaler);
    }


} // namespace timer2


} // namespace timers
