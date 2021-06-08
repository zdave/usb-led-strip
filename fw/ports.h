#pragma once

#include <avr/io.h>

// USB port defines in usbconfig.h

#define STATUS_DDR DDRB
#define STATUS_PORT PORTB
#define STATUS_GREEN 0x2
#define STATUS_ORANGE 0x4

#define WS2812B_DDR DDRD
#define WS2812B_PORT PORTD
