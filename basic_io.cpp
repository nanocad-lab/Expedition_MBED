/*
 * Implementation of a family of print and scan functions
 *
 * Written by Zimin Wang
 */
#include <stdarg.h>
#include <stdio.h>
#include "mbed.h"
#include "EthernetInterface.h"
#include "mmap.h"
#include "signal.h"
#include "pinout.h"
#include "basic_io.h"
#include "locks.h"

#define VERSION 0

const int SERVER_PORT = 7;
#if VERSION == 0
const char *const LOCAL_ADDRESS = "192.168.1.64";
const char *const REMOTE_ADDRESS = "192.168.1.128";
#else
const char *const LOCAL_ADDRESS = "192.168.1.128";
const char *const REMOTE_ADDRESS = "192.168.1.64";
#endif
const char *const MASK = "255.255.255.0";
const char *const GATEWAY = "192.168.1.1";


static EthernetInterface eth;
static UDPSocket server;
static Endpoint remote;
static bool ethernet_open = false;


// buffer for read from or write to terminal
static char term_txbuffer[BUF_SIZE];
static char term_rxbuffer[BUF_SIZE];

// Each time we can only read/write 4 bytes of data from/to ram buffer through JTAG
union byte_chunk_union {
    unsigned int word;
    char bytes[4];
};


// Actual function called when read from or write to terminal.
// Send buffer to stdout.
static void term_sendbuffer(const char *buffer, size_t size);
// Read stdin and write a maximum of size bytes data to buffer.
static void term_readbuffer(char *buffer, size_t size);
// copy contents of WRITEBUF to another buffer
static int jtag_writebuffer(JTAG *pJtag, char *buffer);
// copy cotents of buffer to READBUF
static int jtag_readbuffer(const char *buffer, JTAG *pJtag);


int mbed_printf(const char *format, ...) {
    int size;
    va_list args;
    
    va_start(args, format); 
    size = mbed_vprintf(format, args);
    va_end(args);
       
    return size; 
}

int mbed_vprintf(const char *format, va_list args) {
    int size;
    
    // tx_line is in critical section. We need a mutex to protect it.
    term_write_mutex.lock(); 
    size = vsnprintf(term_txbuffer, BUF_SIZE, format, args);
    term_sendbuffer(term_txbuffer, BUF_SIZE);
    term_write_mutex.unlock();
    
    return size;
}

// prompt user to input data from terminal
int mbed_scanf(const char *format, ...) {
    int size;
    va_list args;
    
    va_start(args, format);
    size = mbed_vscanf(format, args);
    va_end(args);
    
    return size;
}

int mbed_vscanf(const char *format, va_list args) {
    int size;
    
    // term_rxbuffer is in critical section, we need a mutex to protect it.
    term_read_mutex.lock();
    term_readbuffer(term_rxbuffer, BUF_SIZE);
    size = vsscanf(term_rxbuffer, format, args);
    term_read_mutex.unlock();
    return size;   
}

int term_printf(JTAG *pJtag) {
    int len;
    
    // acquire mutex to protect term_txbuffer
    term_write_mutex.lock();
    
    // read data from ram buffer
    len = jtag_writebuffer(pJtag, term_txbuffer);  
    term_sendbuffer(term_txbuffer, BUF_SIZE);
    
    // release locks
    term_write_mutex.unlock();
    return len;
}

int term_scanf(JTAG *pJtag) {
    int len;
    // acquire locks to protect term_rxbuffer
    term_read_mutex.lock();
    
    // read from terminal and write to ram buffer
    term_readbuffer(term_rxbuffer, BUF_SIZE);
    len = jtag_readbuffer(term_rxbuffer, pJtag);
   
    // release locks
    term_read_mutex.unlock();
    return len;
}

void debug_print(JTAG *pJtag) {
    // print the content of ram buffer
    unsigned int value;
    
    for (unsigned int i = WRITEBUF_BEGIN; i < WRITEBUF_END; i+=4) {
        value = pJtag->readMemory(i);
        mbed_printf("%08x\r\n", value);
    }
    mbed_printf("\r\n");
    for (unsigned int i = READBUF_BEGIN; i < READBUF_END; i+=4) {
        value = pJtag->readMemory(i);
        mbed_printf("%08x\r\n", value);
    }
}

static void term_sendbuffer(const char *buffer, size_t size) {
    int i;
    
    i = 0;
    while (i < size) {
        if (buffer[i] != '\0' && pc.writeable()) {
            pc.putc(buffer[i++]);
        } else if (buffer[i] == '\0') {
            break;
        }
    }
    return;
}

static void term_readbuffer(char *buffer, size_t size) {
    int i;
    char temp;

    i = 0;
    while (i < size-1) { // save for null character
        if (pc.readable()) {
            temp = pc.getc();
            if (temp == '\r')
                break;
            else
                buffer[i++] = temp;
        }
    }
    buffer[i] = '\0';
    return;
}

static int jtag_writebuffer(JTAG *pJtag, char *buffer) {
    union byte_chunk_union chunk;
    
    // read data from ram buffer
    bool finished = false;
    int len = 0;
    for (unsigned int i = WRITEBUF_BEGIN; i < WRITEBUF_END; i += 4) {
        JTAG_mutex.lock();
        chunk.word = pJtag->readMemory(i);
        JTAG_mutex.unlock();
        for (int j = 0; j < 4; ++j) {
            if (chunk.bytes[j] != '\0') {
                *buffer++ = chunk.bytes[j];
                ++len;
            }
            else {
                finished = true;
                break;
            }
        }
        if (finished)
            break;
    }
    *buffer = '\0';
    
    return len;
}

static int jtag_readbuffer(const char *buffer, JTAG *pJtag) {
    union byte_chunk_union chunk;
    
    bool finished = false;
    int len = 0;
    for (int i = READBUF_BEGIN; i < READBUF_END; i+= 4) {
        for (int j = 0; j < 4; ++j) {
            if (*buffer) {
                chunk.bytes[j] = *buffer++;
                ++len;
            } else {
                chunk.bytes[j] = '\0';
                finished = true;
            }
        }
        JTAG_mutex.lock();
        pJtag->writeMemory(i, chunk.word);
        JTAG_mutex.unlock();
        if (finished)
            break;
    }

    return len;
}

int init_ethernet() {
    if (ethernet_open == false) {
        int status = eth.init(LOCAL_ADDRESS, MASK, GATEWAY);
        if (status != 0)
            return status;
        status = eth.connect();
        if (status != 0)
            return status;
        mbed_printf("local address is: %s\r\n", LOCAL_ADDRESS);
        
        remote.set_address(REMOTE_ADDRESS, SERVER_PORT);
        status = server.bind(SERVER_PORT);
        if (status != 0)
            return status;
        ethernet_open = true;  
    }
    return 0;
}

void close_ethernet() {
    if (ethernet_open) {
        eth.disconnect();
    }
}

int inet_printf(JTAG *pJtag) {
    // buffer for writing to ethernet
    static char inet_txbuffer[BUF_SIZE];
    static UDPSocket socket_send;
    int size;
    
    // check if ethernet is already initialized
    if (ethernet_open == false) {
        return -1;
    }
    // copy data in ram buffer to inet_txbuffer for sending
    jtag_writebuffer(pJtag, inet_txbuffer);
    
    socket_send.init();
    size = socket_send.sendTo(remote, inet_txbuffer, sizeof(inet_txbuffer));
    socket_send.close();
        
    return size;     
}

int inet_scanf(JTAG *pJtag) {
    // buffer for reading from ethernet
    static char inet_rxbuffer[BUF_SIZE];
    static Endpoint client;
    int size;
    
    // check if ethernet is already initialized
    if (ethernet_open == false) {
        return -1;
    }
    
    size = server.receiveFrom(client, inet_rxbuffer, sizeof(inet_rxbuffer));
    if (size <= 0)
        return size;
    // copy content in inet_rxbuffer to ram buffer
    inet_rxbuffer[size] = '\0';
    size = jtag_readbuffer(inet_rxbuffer, pJtag);
       
    return size;     
}