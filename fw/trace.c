#include "avr_mcu_section.h"
#include "ports.h"
#include "usbdrv.h"

const struct avr_mmcu_vcd_trace_t trace[] _MMCU_ = {
    {AVR_MCU_VCD_SYMBOL("usb_d_minus_in"), .mask = _BV(USBMINUS), .what = (void *)&USBIN},
    {AVR_MCU_VCD_SYMBOL("usb_d_plus_in"), .mask = _BV(USBPLUS), .what = (void *)&USBIN},
    {AVR_MCU_VCD_SYMBOL("usb_d_minus_out"), .mask = _BV(USBMINUS), .what = (void *)&USBOUT},
    {AVR_MCU_VCD_SYMBOL("usb_d_plus_out"), .mask = _BV(USBPLUS), .what = (void *)&USBOUT},
    {AVR_MCU_VCD_SYMBOL("status_green"), .mask = STATUS_GREEN, .what = (void *)&STATUS_PORT},
    {AVR_MCU_VCD_SYMBOL("status_orange"), .mask = STATUS_ORANGE, .what = (void *)&STATUS_PORT},
    {AVR_MCU_VCD_SYMBOL("ws2812b"), .what = (void *)&WS2812B_PORT}};
