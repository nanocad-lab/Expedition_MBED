/*
 * Implementations of two threads created in main.cpp.
 * One thread is used for handling core read/write request, the other thread is for sending an acknowledge interrupt.
 *
 * Written by Zimin Wang
 *  
 */
#include "mbed.h"
#include "GPIOInterrupt.h"

void enable_GPIO_intr(void (*handler)(void)) {
    req_intr.rise(handler);
    req_intr.mode(PullDown);
}

void disable_GPIO_intr(void) {
    req_intr.disable_irq();
    return;  
}
