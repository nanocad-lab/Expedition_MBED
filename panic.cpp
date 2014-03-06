/*
 * Written by Zimin Wang
 *  
 */
#include "mbed.h"
#include "pinout.h"
#include "power.h"
#include "panic.h"
#include "basic_io.h"

// When error happens, print error message, power down the core and terminate
void panic(const char *format, ...)
{
    // print error message
    va_list args;
    va_start(args, format);
    
    mbed_vprintf(format, args);
    va_end(args);
    
    // close ethernet interface
    close_ethernet();
    
    wait(2);
    power_down();
    
    exit(1);   
}