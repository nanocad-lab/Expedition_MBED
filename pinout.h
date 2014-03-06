#ifndef PINOUT_H
#define PINOUT_H

#include <InterruptIn.h>

extern LocalFileSystem local;
extern Serial pc;

// TI DAC7578 interface
//extern BusOut CLR_BAR(p5, p7);
//extern BusOut LDAC_BAR(p6, p8);
extern I2C dac_i2c; //sda, scl

extern I2C lcd;

// LED Indicators
extern DigitalOut power_indicator;
extern DigitalOut power_error_indicator;
extern DigitalOut wait_indicator;

// To Chip
extern DigitalOut PORESETn;
extern DigitalOut CORERESETn;

// From Chip
extern DigitalIn  HCLK_div_down;
extern DigitalIn  RO_clock_out;

// GPIO to/from Chip
//extern BusInOut GPIO(p7, p16, p15);
//#define GPIO_int_pin (p15)
//#define GPIO_ack_pin (p16)
extern InterruptIn req_intr;
extern DigitalOut ack_intr;

// Analog in from amplifier (for power measurement)
extern AnalogIn  meas_sen;
extern AnalogIn  meas_mem2;
extern AnalogIn  meas_mem1;
extern AnalogIn  meas_core;
extern DigitalOut gain_ctrl;

// Scan
extern DigitalOut scan_data_in;
extern DigitalOut scan_phi;
extern DigitalOut scan_phi_bar;
extern DigitalOut scan_load_chain;
extern DigitalOut scan_load_chip;
extern DigitalIn  scan_data_out;

// JTAG
extern DigitalOut TCK;
extern DigitalOut TMS;
extern DigitalOut TDI;
extern DigitalIn  TDO;


#define POWER_UP_TIME 0.01

// DAC0, ADDR=1
#define ADVDD           ADDR_1,CHAN_A
#define PADVDD          ADDR_1,CHAN_B
#define DVDD            ADDR_1,CHAN_C
#define ADVDD2          ADDR_1,CHAN_D
#define COREVDD         ADDR_1,CHAN_E
#define DVDD2           ADDR_1,CHAN_F
#define SENSORVDD       ADDR_1,CHAN_G
#define SENSORLOWVDD    ADDR_1,CHAN_H

// DAC1, ADDR=0
#define SENSORSTRESSVDD ADDR_0,CHAN_A
#define CLOCKVDD        ADDR_0,CHAN_B
#define MEM1VDD         ADDR_0,CHAN_C
#define MEM2VDD         ADDR_0,CHAN_D
#define PLLAVDD         ADDR_0,CHAN_E
#define RING_OSC_NBIAS  ADDR_0,CHAN_F

// 20 MHz Crystal
#define PLL_REF 20000

#endif