/*
 * Written by Zimin Wang
 *  
 */
#ifndef PANIC_H_
#define PANIC_H_

// When some error happens, print error message and terminate
extern void panic(const char *format, ...);

#endif