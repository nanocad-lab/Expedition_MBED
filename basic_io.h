/*
 * A family of print and scan functions.
 * 
 * Written by Zimin Wang.
 */

#ifndef BASIC_IO_H
#define BASIC_IO_H

#include <stdarg.h>
#include "jtag.h"

// IO functions to read/write data from/to terminal to mbed program.
// These functions are used as a replacement to pc.printf() and pc.scanf()
extern int mbed_printf(const char *format, ...);
extern int mbed_scanf(const char *format,...);
extern int mbed_vprintf(const char *format, va_list args);
extern int mbed_vscanf(const char *format, va_list args);

// IO functions to read from ram buffer within Cortex-M3 core and write to peripherals of the testboard,
// or read data from peripherals of the testboard and write them to the ram buffer of Cortex-M3 core.
// foo_printf below reads from ram buffer and write data to peripheral "foo".
// foo_scanf below reads from peripheral "foo" and write data to ram buffer.
extern int term_printf(JTAG *pJtag);
extern int term_scanf(JTAG *pJtag);
extern int xbee_printf(const char *format, ...);
extern int xbee_scanf(const char *format, ...);
extern int usb_printf(const char *format, ...);
extern int usb_scanf(const char *format, ...);
extern int inet_printf(JTAG *pJtag);
extern int inet_scanf(JTAG *pJtag);

extern int init_ethernet();
extern void close_ethernet();

// read from ram buffer and print for debug use
extern void debug_print(JTAG *pJtag);

#endif