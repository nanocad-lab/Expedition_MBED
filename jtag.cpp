#include "jtag.h"
#include "mbed.h"
#include "pinout.h"
#include "mmap.h"
#include "panic.h"
#include "basic_io.h"

using namespace std;



//-----------------------------------------
// Memory

unsigned int JTAG::memRead(unsigned int baseaddr, unsigned int readdata[], int size, bool check, bool print)
{
    unsigned int mismatch = 0;

    writeBanksel(0);
    writeAPACC(0x23000052, AP_CSW);

    unsigned int addr = baseaddr;

    int i = 0;
    while(i*1024 < size) {
        writeAPACC(addr, AP_TAR, false);

        readAPACC(AP_DRW, false, false);
        unsigned int j;
        for(j=1024*i+1; j<1024*(i+1) && j<size; j++) {
            unsigned int word = readAPACC(AP_DRW, false, false);
            if(check) {
                if(readdata[j-1] != word) {
                    mismatch++;
                    if(print) {
                        mbed_printf("Mismatch line %x, was %x, expected %x\r\n", j-1, word, readdata[j-1]);
                    }
                }
            }
            readdata[j-1] = word;
        }
        unsigned int word = rdBuff(false);
        if(check) {
            if(readdata[j-1] != word) {
                mismatch++;
                if(print) {
                    mbed_printf("Mismatch line %x, was %x, expected %x\r\n", j-1, word, readdata[j-1]);
                }
            }
        }
        readdata[j-1] = word;

        addr = addr + 1024*4;
        i++;
    }
    return mismatch;
}

void JTAG::memWrite(unsigned int baseaddr, unsigned int writedata[], int size, bool zero)
{
    if(zero){
       mbed_printf("Called with %x, %x\r\n",  baseaddr, size);
    }
    writeBanksel(0);
    writeAPACC(0x23000052, AP_CSW);

    unsigned int addr = baseaddr;

    int i = 0;
    while(i*1024 < size) {
        writeAPACC(addr, AP_TAR, false);

        for(int j=1024*i; j<1024*(i+1) && j<size; j++) {
            if(zero) {
                writeAPACC(0,            AP_DRW, false);
            } else {
                writeAPACC(writedata[j], AP_DRW, false);
            }
        }

        addr = addr + 1024*4;
        i++;
    }
}

unsigned int JTAG::readMemory(unsigned int address)
{
    writeBanksel(0);
    writeAPACC(0x23000052, AP_CSW);
    writeAPACC(address, AP_TAR);
    return readAPACC(AP_DRW);
}

void JTAG::writeMemory(unsigned int address, unsigned int value)
{
    writeBanksel(0);
    writeAPACC(0x23000052, AP_CSW);
    writeAPACC(address, AP_TAR);
    writeAPACC(value, AP_DRW);
    //rdBuff();
}

int JTAG::loadProgram()
{
    unsigned int address;
    unsigned int value;
    //dual_printf("Halting Core");
    PowerupDAP();

    address = DHCSR_ADDR;
    value = DHCSR_DBGKEY | DHCSR_C_HALT | DHCSR_C_DEBUGEN;
    writeMemory(address, value);
    value = readMemory(address);
    if (! ((value & DHCSR_C_HALT) && (value & DHCSR_C_DEBUGEN)) ) {
        panic("cannot halt the core, check DHCSR...\r\n");
    }

   // dual_printf("Reading Program HEX");
   // pc.printf("loading program\r\n");
    FILE *fp = fopen("/local/program.txt", "r");
   // pc.printf("Program open\r\n");
    if (fp == NULL) {
        panic("Error in open /local/program.txt\r\n");
    }

    // Similar to MemWrite here
  //  dual_printf("Load prog in Imem");
    writeBanksel(0);
    writeAPACC(0x23000052, AP_CSW);
    unsigned int addr = 0x10000000;

    while(!feof(fp)) {
        writeAPACC(addr, AP_TAR, false);
        for(int j=0; j<1024 && !feof(fp); j++) {
            unsigned int d;
            fscanf(fp, "%X", &d);
            writeAPACC(d, AP_DRW, false);
        }
        addr = addr + 1024*4;
    }

   // dual_printf("Check prog in Imem");
    unsigned int mismatch = 0;

    writeBanksel(0);
    writeAPACC(0x23000052, AP_CSW);
    addr = 0x10000000;

    while(!feof(fp)) {
        writeAPACC(addr, AP_TAR, false);

        readAPACC(AP_DRW, false, false);
        unsigned int j;
        for(j=1; j<1024 && !feof(fp); j++) {
            unsigned int word = readAPACC(AP_DRW, false, false);
            unsigned int d;
            fscanf(fp, "%X", &d);
            if(d != word) {
                mismatch++;
                mbed_printf("Mismatch line %x, was %x, expected %x\r\n", j-1, word, d);
            }
        }
        if(!feof(fp)){
            unsigned int word = rdBuff(false);
            unsigned int d;
            fscanf(fp, "%X", &d);
            if(d != word) {
                mismatch++;
                mbed_printf("Mismatch line %x, was %x, expected %x\r\n", j-1, word, d);
            }
        }
        addr = addr + 1024*4;
    }
    if(mismatch) {
        panic("Mem Load Failed");
    }

    fclose(fp);

    //dual_printf("Map Imem to addr 0");
    writeMemory(set_imem, 1);
    return 0;
}

// ------------------------------------------------
// DP/AP Config

unsigned int JTAG::rdBuff(bool set_ir=true)
{
    if(set_ir) {
        setIR(JTAG_DPACC);
    }
    return shiftData(0, DP_RDBUFF, READ);
}

unsigned int JTAG::readDPACC(unsigned char addr, bool set_ir, bool rdthis)
{
    if(set_ir) {
        setIR(JTAG_DPACC);
    }
    unsigned int retdata = shiftData(0, addr, READ);
    if(rdthis) {
        return rdBuff();
    } else {
        return retdata;
    }
}
unsigned int JTAG::readAPACC(unsigned char addr, bool set_ir, bool rdthis)
{
    if(set_ir) {
        setIR(JTAG_APACC);
    }
    unsigned int retdata = shiftData(0, addr, READ);
    if(rdthis) {
        return rdBuff();
    } else {
        return retdata;
    }
}

// JTAG DP / AP Access Registers (DPACC / APACC)
// http://infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.ddi0413c/Babcdjce.html
void JTAG::writeAPACC(unsigned int data, unsigned char addr, bool set_ir)
{
    if(set_ir) {
        setIR(JTAG_APACC);
    }
    shiftData(data, addr, WRITE);
}

void JTAG::writeDPACC(unsigned int data, unsigned char addr, bool set_ir)
{
    if(set_ir) {
        setIR(JTAG_DPACC);
    }
    shiftData(data, addr, WRITE);
}

void JTAG::writeBanksel(unsigned int banksel, bool set_ir)
{
    if(set_ir) {
        setIR(JTAG_DPACC);
    }
    shiftData(banksel << 4, DP_SELECT, WRITE);
}

// DAP: Debug Access Port
// http://infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.ddi0314h/Babdadfc.html
void JTAG::DAP_enable(void)
{
    setState('r');
    leaveState();
    // write CTRL to enable DAP

    writeDPACC(0x50000000, DP_CTRLSTAT);
    writeDPACC(0x00000000, AP_SELECT);
    writeAPACC(0x23000042, AP_CSW);
}


void JTAG::PowerupDAP()
{
    writeDPACC(0x12345678, DP_SELECT);
    int rd = readDPACC(DP_SELECT);
    if(rd != 0x12000070) {
        //pc.printf("DP SELECT %x", rd);
        //exit(1);
        panic("DP SELECT %x", rd);
    }
    writeDPACC(0xedcba987, DP_SELECT);
    rd = readDPACC(DP_SELECT);
    if(rd != 0xed000080) {
        //pc.printf("DP SELECT %x", rd);
        //exit(1);
        panic("DP SELECT %x", rd);
    }
    writeDPACC(0x10000000, DP_CTRLSTAT);
    rd = readDPACC(DP_CTRLSTAT);//////////////////////////////////////////////////////////////
    if(rd != 0x30000000) {
        //pc.printf("DP CTRL %x", rd);
        //exit(1);
        panic("DP CTRL %x", rd);
    }
    writeDPACC(0x40000000, DP_CTRLSTAT);
    rd = readDPACC(DP_CTRLSTAT);
    if(rd != 0xc0000000) {
        //pc.printf("DP CTRL %x", rd);
        //exit(1);
        panic("DP CTRL %x", rd);
    }
    writeDPACC(0x50000000, DP_CTRLSTAT);
    rd = readDPACC(DP_CTRLSTAT);
    if(rd != 0xf0000000) {
        //pc.printf("DP CTRL %x", rd);
        //exit(1);
        panic("DP CTRL %x", rd);
    }
    writeBanksel(0xf);
    rd = readAPACC(AP_IDR);
    if(rd != 0x24770011) {
        //pc.printf("AP IDR %x", rd);
        //exit(1);
        panic("AP IDR %x", rd);
    }
}

// --------------------------------
// State Manipulation

// IR: instruction register (IR)
void JTAG::setIR(unsigned char A)
{
    setState('i');
    char one = shiftBits(A, 4);
    if(one != 1) {
        //dual_printf("ERROR: JTAG IR");
        //pc.printf("Got %x instead of 1\r\n", one);
        panic("ERROR: JTAG IR. Got %x instead of 1\r\n", one);
    }
    leaveState();
}

//moves to specified state from IDLE (reset from anywhere)
void JTAG::setState(unsigned char c)
{
    switch (c) {
        case 'n':
            break;
        case 'r':
            reset();
            break;
        case 'd':
            TMSHigh();
            clockTicks(1);
            TMSLow();
            clockTicks(2);
            state = 'd';
            break;
        case 'i':
            TMSHigh();
            clockTicks(2);
            TMSLow();
            clockTicks(2);
            state = 'i';
            break;
        default:
            break;
    }
}

//leave from current state to idle state
void JTAG::leaveState(void)
{
    switch (state) {
        case 'n':
            break;
        case 'r':
            TMSLow();
            clockTicks(1);
            state = 'n';
            break;
        case 'i':
            TMSHigh();
            clockTicks(2);
            TMSLow();
            clockTicks(1);
            state = 'n';
            break;
        case 'd':
            TMSHigh();
            clockTicks(2);
            TMSLow();
            clockTicks(1);
            state = 'n';
            break;
        default:
            break;
    }
}

void JTAG::reset(void)
{
    TMSHigh();
    clockTicks(10);
    TMSLow();
    state = 'r';
    return;
}

unsigned int JTAG::readID(void)
{
    setIR(JTAG_IDCODE);
    setState('d');
    unsigned int id = shiftBits(0, 32);
    leaveState();

    return id;
}

// --------------------------------------------
// Data Shifting

unsigned int JTAG::shiftBits(unsigned int data, int n)
{
    unsigned int c=0;
    clockLow();
    int i;
    for (i=0; i<n; i++) {
        if (TDO) {
            c+=(0x1 << i);
        }

        clockTicks(1);

        if ( (data & 1)== 0 ) {
            DataLow();
        } else {
            DataHigh();
        }
        data=data>>1;
    }

    return c;
}

unsigned int JTAG::shiftData(unsigned int data, char addr, bool rw)
{
    bool gotwait = true;
    while(gotwait) {
        gotwait = false;

        setState('d');
        // First 3 bits are either OK/FAULT 010, or WAIT 001
        int okstat = shiftBits(rw, 1);
        okstat |= (shiftBits(addr >> 2, 2) << 1);

        if(okstat == 1) {
            wait_indicator = !wait_indicator;
            leaveState();
            gotwait = true;
        } else if(okstat == 2) {
            // Got OK/FAULT
        } else {
            //dual_printf("invalid OK Stat");
            leaveState();
            //exit(1);
            panic("%s\r\n", "invalid OK Stat");
        }
    }

    unsigned int retdata = shiftBits(data, 32);
    leaveState();
    return retdata;
}


// ----------------------------------
// Toggle Functions

void JTAG::DataLow(void)
{
    wait_us(delay);
    TDI = 0;
}
void JTAG::DataHigh(void)
{
    wait_us(delay);
    TDI = 1;
}

void JTAG::clockLow(void)
{
    wait_us(delay);
    TCK = 0;
}

void JTAG::clockHigh(void)
{
    wait_us(delay);
    TCK = 1;
}

void JTAG::clockTicks(unsigned char c)
{
    int i;
    clockLow();
    for (i=0; i<c; i++) {
        clockLow();
        clockHigh();
    }
    clockLow();
}

// JTAG: TMS (Test Mode Select)
// http://en.wikipedia.org/wiki/Joint_Test_Action_Group
void JTAG::TMSHigh(void)
{
    wait_us(delay);
    TMS = 1;
}

void JTAG::TMSLow(void)
{
    wait_us(delay);
    TMS = 0;
}

// --------------------------------
// Initializing and Config

JTAG::JTAG()
{
    //TDO.mode(PullUp);
    delay = 0;
    TMS = 0;
    TCK = 0;
    TDI = 0;
    reset();
    leaveState();
    return;
}

void JTAG::setJTAGspeed(int speed)
{
    delay = 1000/speed;
    return;
}
