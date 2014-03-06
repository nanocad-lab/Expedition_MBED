#ifndef DAC_H_
#define DAC_H_

#include "mbed.h"
#include "pinout.h"

#define ADDR_0 0x90
#define ADDR_1 0x94
#define ADDR_F 0x98

#define CHAN_A 0x30
#define CHAN_B 0x31
#define CHAN_C 0x32
#define CHAN_D 0x33
#define CHAN_E 0x34
#define CHAN_F 0x35
#define CHAN_G 0x36
#define CHAN_H 0x37

extern void power_chan(char i2caddr, char chan, float voltage);

#endif
