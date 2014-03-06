#include "mbed.h"
//#include "scan.h"
#include "pinout.h"
#include "jtag.h"
#include "mmap.h"

// Main utility function, this function will automatically
// round fMHz to multiple of 5MHz.
// Current supported range: 105MHz - 625MHz
int set_pll_frequency (int fMHz, JTAG &jtag);

unsigned long long calc_pll_freqs(unsigned int prediv, unsigned int multint_upper, unsigned int multint, 
                            unsigned int range_upper, unsigned int rangea, unsigned int rangeb);

int get_binline_by_num(char* filename, int linenum);

void scan_pll(unsigned int prediv, unsigned int multint_upper, unsigned int multint, 
              unsigned int range_upper, unsigned int rangea, unsigned int rangeb);
void jtag_pll(JTAG &jtag, unsigned int prediv, unsigned int multint_upper, unsigned int multint, 
              unsigned int range_upper, unsigned int rangea, unsigned int rangeb);
unsigned int fftune(bool change_rangeab);
unsigned long long lftune();
unsigned int lftune_hi();
unsigned int lftune_lo();
