#include "mbed.h"
#include "pinout.h"

LocalFileSystem local("local");
Serial pc(USBTX, USBRX);//tx, rx => for debugging purposes

// TI DAC7578 interface
//static BusOut CLR_BAR(p5, p7);
//static BusOut LDAC_BAR(p6, p8);
I2C dac_i2c(p9, p10); //sda, scl

I2C lcd(p9, p10);

// LED Indicators
DigitalOut power_indicator (LED1);
DigitalOut power_error_indicator (LED4);
DigitalOut wait_indicator (LED2);

// To Chip
DigitalOut PORESETn(p11);
DigitalOut CORERESETn(p12);

// From Chip
DigitalIn  HCLK_div_down(p5);
DigitalIn  RO_clock_out(p6);

// GPIO to/from Chip
//static BusInOut GPIO(p7, p16, p15);
InterruptIn req_intr(p15);
DigitalOut ack_intr(p16);

// Analog in from amplifier (for power measurement)
AnalogIn  meas_sen(p17);
AnalogIn  meas_mem2(p18);
AnalogIn  meas_mem1(p19);
AnalogIn  meas_core(p20);
DigitalOut gain_ctrl(p8);

// Scan
DigitalOut scan_data_in(p30);
DigitalOut scan_phi(p29);
DigitalOut scan_phi_bar(p13);
DigitalOut scan_load_chain(p14);
DigitalOut scan_load_chip(p26);
DigitalIn  scan_data_out(p25);

// JTAG
DigitalOut TCK(p24);
DigitalOut TMS(p23);
DigitalOut TDI(p22);
DigitalIn  TDO(p21);