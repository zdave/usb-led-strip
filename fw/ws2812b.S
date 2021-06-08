#include <avr/io.h>
#include "ports.h"
#include "usbconfig.h"

    tmp = 0
    zero = 1

// bool ws2812b_write(const uint8_t *begin, const uint8_t *end)

    .text
    .global ws2812b_write
    .type ws2812b_write, @function
ws2812b_write:

    beginl = 24
    beginh = 25
    endl = 22
    endh = 23

    high = 18

    movw xl, beginl
    ser high

    // Assuming 12MHz clock here...
#   if F_CPU != 12000000ul
#       error
#   endif

    // We disable interrupts here as timing is important. But we can't ignore
    // USB interrupts for too long: from a comment in usbdrvasm12.inc, the max
    // interrupt disable time is 25 cycles. So we check each time around the
    // loop (which is 14 cycles long) if a USB interrupt is pending. If one is
    // pending, we break out, handle the interrupt, then return false. The
    // caller is responsible for waiting 50us and then calling again to retry.
    cli

    // From the datasheet:
    // 0: 0.4us +/-0.15us high, 0.85us +/-0.15us low
    // 1: 0.8us +/-0.15us high, 0.45us +/-0.15us low
    // At 12MHz, 1 cycle is ~0.083us. The following timings are well within
    // spec and seem to work fine:
    // 0: 4cyc ~= 0.33us high (~0.07us short), 10cyc ~= 0.83us low (~0.02us short)
    // 1: 9cyc ~= 0.75us high (~0.05us short), 5cyc ~= 0.42us low (~0.03us short)
0:
    out _SFR_IO_ADDR(WS2812B_PORT), high    // 0
    nop                                     // 1
    ld tmp, x+                              // 2, 3
    out _SFR_IO_ADDR(WS2812B_PORT), tmp     // 4
    nop                                     // 5
    nop                                     // 6
    nop                                     // 7
    cp endl, xl                             // 8
    out _SFR_IO_ADDR(WS2812B_PORT), zero    // 9
    cpc endh, xh                            // 10
    sbis _SFR_IO_ADDR(USB_INTR_PENDING), USB_INTR_PENDING_BIT // 11
    brne 0b                                 // 12, 13

    sei

    // Z flag will be set iff we managed to finish...
    in r24, _SFR_IO_ADDR(SREG)
    lsr r24
    andi r24, 1

    ret

    .size ws2812b_write, . - ws2812b_write
