#ifndef LOCKS_H_
#define LOCKS_H_

#include "rtos.h"

extern Mutex term_write_mutex;
extern Mutex term_read_mutex;
extern Mutex JTAG_mutex;

#endif