/*
This is the JTAG driver file for mbed master
Refer to buspriate + openOCD
*/

// Addresses

// DHCSR: Debug Halting Control and Status Register.
// http://infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.ddi0337e/CEGCJAHJ.html
#ifndef JTAG_H
#define JTAG_H

#define DHCSR_ADDR 0xE000EDF0

#define DCRSR_ADDR 0xE000EDF4
#define DCRDR_ADDR 0xE000EDF8
#define DEMCR_ADDR 0xE000EDFC

#define DHCSR_DBGKEY 0xA05F0000
#define DHCSR_S_RESET_ST 0x2000000
#define DHCSR_S_RETIRE_ST 0x1000000
#define DHCSR_S_LOCKUP 0x80000
#define DHCSR_S_SLEEP 0x40000
#define DHCSR_S_HALT 0x20000
#define DHCSR_S_REGRDY 0x10000
#define DHCSR_C_SNAPSTALL 0x20
#define DHCSR_C_MASKINTS 0x8
#define DHCSR_C_STEP 0x4
#define DHCSR_C_HALT 0x2
#define DHCSR_C_DEBUGEN 0x1

#define DCRSR_REGWnR 0x10000
#define DCRSR_xPSR 0x10
#define DCRSR_MSP  0x11
#define DCRSR_PSP  0x12
#define DCRSR_CONTROL  0x14

#define DEMCR_TRCENA 0x1000000
#define DEMCR_MON_REQ 0x80000
#define DEMCR_MON_STEP 0x40000
#define DEMCR_MON_PEND 0x20000
#define DEMCR_MON_EN 0x10000
#define DEMCR_VC_HARDERR 0x400
#define DEMCR_VC_INTERR 0x200
#define DEMCR_VC_BUSERR 0x100
#define DEMCR_VC_STATERR 0x80
#define DEMCR_VC_CHKERR 0x40
#define DEMCR_VC_NOCPERR 0x20
#define DEMCR_VC_MMERR 0x10
#define DEMCR_VC_CORERESET 0x1



#define AP 1
#define DP 0

#define READ 1
#define WRITE 0

#define DP_CTRLSTAT 0x4
#define DP_SELECT 0x8
#define DP_RDBUFF 0xC

#define AP_CSW 0x0
#define AP_TAR 0x4
#define AP_SELECT 0x8
#define AP_DRW 0xC
#define AP_BD0 0x10
#define AP_BD1 0x14
#define AP_BD2 0x18
#define AP_BD3 0x1C
#define AP_CFG 0xF4
#define AP_BASE 0xF8
#define AP_IDR 0xFC

#define JTAG_ABORT 0x8
#define JTAG_DPACC 0xA
#define JTAG_APACC 0xB
#define JTAG_IDCODE 0xE
#define JTAG_BYPASS 0xF


#define SW_DP_ACK_OK 1
#define SW_DP_ACK_WAIT 2
#define SW_DP_ACK_FAULT 4



class JTAG
{
public:

// Memory
    unsigned int memRead(unsigned int baseaddr, unsigned int readdata[], int size, bool check=false, bool print=false);
    void memWrite(unsigned int baseaddr, unsigned int writedata[], int size, bool zero=false);
    unsigned int readMemory(unsigned int address);
    void writeMemory(unsigned int address, unsigned int value);
    int loadProgram();

// ------------------------------------------------
// DP/AP Config
    unsigned int rdBuff(bool set_ir);
    unsigned int readDPACC(unsigned char addr, bool set_ir=true, bool rdthis=true);
    unsigned int readAPACC(unsigned char addr, bool set_ir=true, bool rdthis=true);
    void writeAPACC(unsigned int data, unsigned char addr, bool set_ir=true);
    void writeDPACC(unsigned int data, unsigned char addr, bool set_ir=true);
    void writeBanksel(unsigned int banksel, bool set_ir=true);
    void DAP_enable(void);
    void PowerupDAP();

// --------------------------------
// State Manipulation
    void setIR(unsigned char A);
    void setState(unsigned char c);
    void leaveState(void);
    void reset(void);
    unsigned int readID(void);

// --------------------------------------------
// Data Shifting
    unsigned int shiftBits(unsigned int data, int n);
    unsigned int shiftData(unsigned int data, char addr, bool rw);

// ----------------------------------
// Toggle Functions
    void DataLow(void);
    void DataHigh(void);
    void clockLow(void);
    void clockHigh(void);
    void clockTicks(unsigned char c);
    void TMSHigh(void);
    void TMSLow(void);

// --------------------------------
// Initializing and Config
    JTAG();
    void setJTAGspeed(int speed);



    char state; // n=null, r=reset, d=data, i=instruction
    int delay; // wait time for each signal switching, in us
};

#endif
