#include <avr/interrupt.h>
#include <avr/io.h>
#include <stdbool.h>
#include <stdint.h>
#include <util/delay.h>
#include "ports.h"
#include "usbdrv.h"
#include "ws2812b.h"

#define USB_REQ_WRITE_BUF 0 // wIndex: index to start at in buffer
#define USB_REQ_UPDATE_LEDS 1 // No data

#define LEDS_PER_STRIP 27

// One bit in each uint8_t for each strip. 24 bits per LED.
static uint8_t buf[LEDS_PER_STRIP * 24];

static uint8_t *write_ptr;
static usbMsgLen_t write_size;

// Set when update request received. Cleared on successful update.
static bool update_leds;

// Assumes prescale of 8
#define TIMER_TICKS_FROM_US(US) ((F_CPU * (US)) / 8000000ul)

static void reset_timer(void)
{
    TCNT0 = 0xff - TIMER_TICKS_FROM_US(50); // 50us to overflow
    TIFR0 = _BV(TOV0); // Clear overflow bit
    TIMSK0 |= _BV(TOIE0); // Enable overflow interrupt
}

static void init_timer(void)
{
    TCCR0B = _BV(CS01); // Start timer, prescaled by 8
    reset_timer();
}

ISR(TIMER0_OVF_vect)
{
    TIMSK0 &= ~_BV(TOIE0); // Disable overflow interrupt
}

static bool timer_elapsed(void)
{
    // Timer has elapsed if overflow interrupt has been disabled
    return bit_is_clear(TIMSK0, TOIE0);
}

USB_PUBLIC usbMsgLen_t usbFunctionSetup(uchar data[8])
{
    usbRequest_t *r = (usbRequest_t *)data;
    switch (r->bRequest)
    {
    case USB_REQ_WRITE_BUF:
    {
        if (r->wIndex.word >= sizeof(buf))
            return 0;

        write_ptr = buf + r->wIndex.word;
        write_size = (usbMsgLen_t)r->wLength.word;

        size_t max_write_size = sizeof(buf) - r->wIndex.word;
        if (write_size > max_write_size)
            write_size = (usbMsgLen_t)max_write_size;

        // Call usbFunctionWrite
        return USB_NO_MSG;
    }
    case USB_REQ_UPDATE_LEDS:
        update_leds = true;
        STATUS_PORT |= STATUS_ORANGE;
        return 0;
    default:
        // Ignore unknown request
        return 0;
    }
}

USB_PUBLIC uchar usbFunctionWrite(uchar *data, uchar len)
{
    // Clamp to remaining size
    if (len > write_size)
        len = (uchar)write_size;

    for (uchar i = 0; i != len; ++i)
        *(write_ptr++) = data[i];

    write_size -= len;
    return write_size == 0;
}

int main(int argc, char **argv)
{
    STATUS_DDR |= STATUS_GREEN | STATUS_ORANGE;
    WS2812B_DDR = 0xff;

    init_timer();

    usbInit();
    usbDeviceDisconnect();
    _delay_ms(250);
    usbDeviceConnect();

    STATUS_PORT |= STATUS_GREEN;

    sei();

    for (;;)
    {
        usbPoll();

        // Only update LEDs if it has been long enough since the last update
        if (update_leds && timer_elapsed())
        {
            if (ws2812b_write(buf, buf + sizeof(buf)))
            {
                update_leds = false;
                STATUS_PORT &= ~STATUS_ORANGE;
            }

            // Must always wait between ws2812b_write calls, whatever the
            // return value
            reset_timer();
        }
    }

    return 0;
}
