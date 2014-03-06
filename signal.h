#ifndef SIGNAL_H_
#define SIGNAL_H_

#define TERM_PRINT_REQ   (0U)
#define TERM_SCAN_REQ    (1U)
#define INET_PRINT_REQ   (2U)
#define INET_SCAN_REQ    (3U)
#define PANIC_REQ        (15U)

#define PRINT_ACK   (0U)
#define SCAN_ACK    (1U)

// core send a print or scan request to mbed
#define SIG_DISPATCH    (0x1)
#define SIG_TERM_WRITE  (0x2)
#define SIG_TERM_READ   (0x4)
#define SIG_INET_WRITE  (0x8)
#define SIG_INET_READ   (0x10)

// user type debug, wake up a thread to print content of ram buffer for debug
#define SIG_DEBUG (0x20)

#endif