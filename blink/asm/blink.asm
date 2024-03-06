; blink.asm
; Blink an LED connected to Port D2


; Load hardware definition
.NOLIST
.include "./m328Pdef.inc"
.LIST


;Setup
    ;Stack
    ldi r16, LOW(RAMEND)        ; Load low byte of stack pointer
    out SPL,r16
    ldi r16, HIGH(RAMEND)       ; Load high byte of stack pointer
    out SPH,r16


    ;LED pin
    sbi DDRD, 6


;Loop
loop:
    sbi PORTD, 6                ; Turn LED on
    rcall delay_50ms            ; Call delay subroutine once

    cbi PORTD, 6                ; Turn LED off
    ldi r23, 19                 ; Call delay subroutine 19 times (19 * 50ms = 950ms)
stay_off:
    rcall delay_50ms
    dec r23
    brne stay_off

    rjmp loop                   ; Forever and ever...


delay_50ms:
    ldi  r24, LOW(12500)
    ldi  r25, HIGH(12500)
L1:
    sbiw r24, 1
    brne L1
    ret
