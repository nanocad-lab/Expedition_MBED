#include "mbed.h"
#include "rtos.h"
#include "power.h"
#include "pinout.h"
#include "pll.h"
#include "jtag.h"
#include "mmap.h"
#include "GPIOInterrupt.h"
#include "basic_io.h"
#include "panic.h"
#include "main.h"
#include "signal.h"
#include "locks.h"

JTAG jtag;

void init_hw(void);
void dispatch_loop(void const *args);
void term_write_loop(void const *args);
void term_read_loop(void const *args);
void inet_write_loop(void const *args);
void inet_read_loop(void const *args);
void debug_loop(void const *args);

Thread *dispatch_thread_ptr;
Thread *term_write_thread_ptr;
Thread *term_read_thread_ptr;
Thread *inet_write_thread_ptr;
Thread *inet_read_thread_ptr;

void req_intr_handler(void);

int main()
{
    init_hw();
    
    // We check if JTAG works well by checking idcode returned by issuing a test instruction and read result idcode back.
    jtag.reset();
    jtag.leaveState();
    int idcode = jtag.readID();
    if(idcode != 0x4ba00477) {
        panic("ERROR: IDCode %X\r\n", idcode);
    }
    
    // create working threads first
    Thread dispatch_thread(dispatch_loop, NULL);
    Thread term_write_thread(term_write_loop, NULL);
    Thread term_read_thread(term_read_loop, NULL);
    Thread inet_write_thread(inet_write_loop, NULL);
    Thread inet_read_thread(inet_read_loop, NULL);
    Thread debug_thread(debug_loop, NULL);
    
    dispatch_thread_ptr = &dispatch_thread;
    term_write_thread_ptr = &term_write_thread;
    term_read_thread_ptr = &term_read_thread;
    inet_write_thread_ptr = &inet_write_thread;
    inet_read_thread_ptr = &inet_read_thread;
    // enable GPIO interrupt
    enable_GPIO_intr(&req_intr_handler);

    jtag.reset();
    jtag.leaveState();
    jtag.PowerupDAP();
    // setup necessary internal clock source selection
    jtag.writeMemory(intclk_source, 2);
    jtag.writeMemory(extclk_source, 1);
    jtag.writeMemory(ext_div_by, 10);
    power_core(1);
    
    //set_pll_frequency (200, jtag); // default internal frequency is 80MHz
    
    // Begin to load program
    mbed_printf("Begining loading program.\r\n");
    if (jtag.loadProgram()) {
        mbed_printf("Load Failed!\r\n");
    } else {
        mbed_printf("Load Succeed!\r\n");
        CORERESETn = 0;
        CORERESETn = 1;        
        char line[80];
        
        while (1) {  
            mbed_printf("Type 'quit' to quit.\r\n");
            mbed_scanf("%s", line);
            if (strncmp(line, "quit", 80) == 0)
                break;
            else if (strncmp(line, "debug", 80) == 0)
                debug_thread.signal_set(SIG_DEBUG);  
        }
        
    }    
    
    dispatch_thread.terminate();
    term_write_thread.terminate();
    term_read_thread.terminate();
    inet_write_thread.terminate();
    inet_read_thread.terminate();
    debug_thread.terminate();
    
    jtag.reset();

    mbed_printf("Powering Down\r\n");
    power_down();
    mbed_printf("Done.\r\n");
    while (1)
        ; 
}

void init_hw(void) {
    // Ethernet initialization
    
    if (init_ethernet() != 0) {
        panic("Ethernet initialization failed.\r\n");
    }

    float core_volt = 1;
    power_down();
    power_up(core_volt); // Power Up Chip
    mbed_printf("Powered up!\r\n");

    PORESETn = 0;
    CORERESETn = 0;
    wait_us(100);
    PORESETn = 1;
    CORERESETn = 1;
}


void req_intr_handler(void) {
    static bool init = true;
    
    if (init) {
        jtag.reset();
        jtag.leaveState();
        jtag.PowerupDAP();
        init = false;
    }
    dispatch_thread_ptr->signal_set(SIG_DISPATCH); 
    return;
}

void dispatch_loop(void const *args) {
    while (1) {
        Thread::signal_wait(SIG_DISPATCH);
        JTAG_mutex.lock();
        // check request type and wake up corresponding threads
        unsigned int type = jtag.readMemory(IO_TYPE);
        JTAG_mutex.unlock();
        
        switch(type) {
        case PANIC_REQ:
            //fall through
        case TERM_PRINT_REQ:
            term_write_thread_ptr->signal_set(SIG_TERM_WRITE);
            break;
        case TERM_SCAN_REQ:
            term_read_thread_ptr->signal_set(SIG_TERM_READ);
            break;
        case INET_PRINT_REQ:
            inet_write_thread_ptr->signal_set(SIG_INET_WRITE);
            break;
        case INET_SCAN_REQ:
            inet_read_thread_ptr->signal_set(SIG_INET_READ);
            break;
        default:
            mbed_printf("Unsupported request: %08x\r\n", type);
            continue;
        }
    }
}

void term_write_loop(void const *args) {
    while (1) {
        Thread::signal_wait(SIG_TERM_WRITE);
        term_printf(&jtag);
        JTAG_mutex.lock();
        jtag.writeMemory(ACK_TYPE, PRINT_ACK);
        // generate acknowledge signal
        ack_intr = 1;
        ack_intr = 0;
        JTAG_mutex.unlock();    
    }
}

void term_read_loop(void const *args) {
    while (1) {
        Thread::signal_wait(SIG_TERM_READ);
        term_scanf(&jtag);
        JTAG_mutex.lock();
        jtag.writeMemory(ACK_TYPE, SCAN_ACK);
        // generate acknowledge signal
        ack_intr = 1;
        ack_intr = 0;
        JTAG_mutex.unlock();    
    }
}

void inet_write_loop(void const *args) {
    while (1) {
        Thread::signal_wait(SIG_INET_WRITE);
        if (inet_printf(&jtag) <= 0)
            panic("inet_printf failed.\r\n");
            
        JTAG_mutex.lock();
        jtag.writeMemory(ACK_TYPE, PRINT_ACK);
        // generate acknowledge signal
        ack_intr = 1;
        ack_intr = 0;
        JTAG_mutex.unlock();  
    }
}

void inet_read_loop(void const *args) {
    while (1) {
        Thread::signal_wait(SIG_INET_READ);
        if (inet_scanf(&jtag) <= 0)
            panic("inet_scanf failed.\r\n");
            
        JTAG_mutex.lock();
        jtag.writeMemory(ACK_TYPE, SCAN_ACK);
        // generate acknowledge signal
        ack_intr = 1;
        ack_intr = 0;
        JTAG_mutex.unlock();    
    }
}

void debug_loop(void const *args) {
    static bool init = true;
    while (1) {
        Thread::signal_wait(SIG_DEBUG);
        if (init) {
            JTAG_mutex.lock();
            jtag.reset();
            jtag.leaveState();
            jtag.PowerupDAP();
            init = false;
            JTAG_mutex.unlock();
        }
        debug_print(&jtag);
    }
}
