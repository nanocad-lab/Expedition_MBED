/*
 * Written by Zimin Wang
 *  
 */
#ifndef GPIO_INTERRUPT_H_
#define GPIO_INTERRUPT_H_

#include "pinout.h"
#include <InterruptIn.h>

extern InterruptIn req_intr;
extern DigitalOut ack_intr;
// enable and disable interrupt from GPIO
extern void enable_GPIO_intr(void (*handler)(void));
extern void disable_GPIO_intr(void);

#endif