#include "pll.h"

unsigned long long calc_pll_freqs(unsigned int prediv, unsigned int multint_upper, unsigned int multint, 
                            unsigned int range_upper, unsigned int rangea, unsigned int rangeb){

    if(prediv < 1 || prediv > 32){
        pc.printf("Prediv out of 1-32 range\r\n");
    }
    if(multint_upper < 1 || multint_upper > 4){
        pc.printf("Multint_upper out of 1-4 range\r\n");
    }
    if(multint < 1 || multint > 255){
        pc.printf("Multint out of 1-255 range\r\n");
    }
    if(multint_upper * multint > 255){
        pc.printf("Multint_upper * Multint out of 1-255 range\r\n");
    }
    if(!(range_upper == 1 || range_upper == 2 || range_upper == 4)){
        pc.printf("Range_upper out of 1,2,4 range\r\n");
    }
    if(rangea < 1 || rangea > 32){
        pc.printf("RangeA out of 1-32 range\r\n");
    }
    if(rangeb < 1 || rangeb > 32){
        pc.printf("RangeB out of 1-32 range\r\n");
    }
    
    int dco = (PLL_REF / prediv) * multint_upper * multint;
    if(dco < 2500000 || dco > 5000000){
        pc.printf("Dco=%d out of 2.5G-5G range\r\n",dco);
    }
    int internal = dco / multint_upper;
    if(internal < 9800 || internal > 3200000){
       pc.printf("internal=%d out of 9.8M-3.2G range\r\n",internal);
    }
    int prescale_dco = dco / range_upper;
    if(prescale_dco > 3200000){
       pc.printf("Prescale DCO out of 0-3.2G range\r\n");
    }
    int out_a = prescale_dco / rangea;
    if(out_a < 20000 || out_a > 3000000){
        pc.printf("Out A out of 20M-3.0G range\r\n");
    }
    int out_b = prescale_dco / rangeb;
    if(out_b < 20000 || out_b > 3000000){
        pc.printf("Out B out of 20M-3.0G range\r\n");
    }
    
    return out_a;
}

int get_binline_by_num(char* filename, int linenum){
    char value[30];
    
    //pc.printf("Opening %s File\r\n", filename);
    FILE *fp = fopen(filename, "r");
    if(!fp){
        pc.printf("ERROR: %s not found\r\n", filename);
    }
    //pc.printf("%s Open\r\n", filename);
        
    int i=1;
    char lineval[255];
    while (!feof(fp)) {
        fscanf(fp, "%s", lineval);
        if(i == linenum){
            strcpy(value, lineval);
            break;
        }
        i++;
    }
            
    //pc.printf("Closing %s File\r\n", filename);
    fclose(fp);
    //pc.printf("File %s Closed\r\n", filename);
    
    return strtol(value, NULL, 2);
}

/*
void scan_pll(unsigned int prediv, unsigned int multint_upper, unsigned int multint, 
              unsigned int range_upper, unsigned int rangea, unsigned int rangeb){
    
    prediv        = get_binline_by_num("/local/Pprediv.txt",  prediv);
    multint_upper = get_binline_by_num("/local/Pmultup.txt",  multint_upper);
    multint       = get_binline_by_num("/local/Pmultint.txt", multint);
    range_upper   = get_binline_by_num("/local/Prangeup.txt", range_upper);
    rangea        = get_binline_by_num("/local/Prange.txt",   rangea);
    rangeb        = get_binline_by_num("/local/Prange.txt",   rangeb);
    
    set_scan_bits("PLL_CE1CCB",   1);
    set_scan_bits("PLL_CE1MPGC1", 1);
    set_scan_bits("PLL_FFTUNE",   fftune(0));
    set_scan_bits("PLL_LFTUNE",   lftune()); // @@@ Fix this. Scan doesn't support more than 32 bits
    set_scan_bits("PLL_INTFBK",   1);
    set_scan_bits("PLL_PREDIV",   prediv);
    set_scan_bits("PLL_MULTINT",  multint_upper << 8 | multint);
    set_scan_bits("PLL_RANGEA",   range_upper << 5 | rangea);
    set_scan_bits("PLL_RANGEB",   range_upper << 5 | rangeb);
}
*/
void jtag_pll(JTAG &jtag, unsigned int prediv, unsigned int multint_upper, unsigned int multint, 
              unsigned int range_upper, unsigned int rangea, unsigned int rangeb){
    
    prediv        = get_binline_by_num("/local/Pprediv.txt",  prediv);
    multint_upper = get_binline_by_num("/local/Pmultup.txt",  multint_upper);
    multint       = get_binline_by_num("/local/Pmultint.txt", multint);
    range_upper   = get_binline_by_num("/local/Prangeup.txt", range_upper);
    rangea        = get_binline_by_num("/local/Prange.txt",   rangea);
    rangeb        = get_binline_by_num("/local/Prange.txt",   rangeb);
    jtag.writeMemory(PLL_CE1CCB,   1);
    jtag.writeMemory(PLL_CE1MPGC1, 1);
    jtag.writeMemory(PLL_FFTUNE,   fftune(0));
    jtag.writeMemory(PLL_LFTUNE_32_0,   lftune_lo());
    jtag.writeMemory(PLL_LFTUNE_40_32,   lftune_hi());
    jtag.writeMemory(PLL_INTFBK,   1);
    jtag.writeMemory(PLL_PREDIV,   prediv);
    jtag.writeMemory(PLL_RANGEA,   range_upper << 5 | rangea);
    jtag.writeMemory(PLL_RANGEB,   range_upper << 5 | rangeb);
    jtag.writeMemory(PLL_MULTINT,  multint_upper << 8 | multint);
}


unsigned int fftune(bool change_rangeab){
    if(change_rangeab){
        return 0x0120;
    }else{
        return 0x0100;
    }
}

unsigned int lftune_hi(){
    return 0x00000050;
}

unsigned int lftune_lo(){
    return 0x40100000;
}

unsigned long long lftune(){
    // 41 bits
    return 0x05040100000L;
}

/*
unsigned long long calc_pll_freqs(unsigned int prediv, unsigned int multint_upper, unsigned int multint, 
                            unsigned int range_upper, unsigned int rangea, unsigned int rangeb)
                            dco = (PLL_REF / prediv) * multint_upper * multint;
                            prescale_dco = dco / range_upper;
                            out_a = prescale_dco / rangea;
*/
int set_pll_frequency (int fMHz, JTAG &jtag) {
  jtag.writeMemory(intclk_source, 0);
  int counter = fMHz / 5;
  int frequency = counter * 5;
  if (frequency >= 625) {
    pc.printf("Frequency > 625 MHz out of range!\r\nClock source changed to HCLK_EXT\r\n");
    return 1;
  } /*
  else if (frequency >= 625) {
    //counter between 125 and 200
    // 20*125/4=625
    // 20*200/4=1G 
    jtag_pll(jtag, 1, 1, counter, 1, 4, 4);
    calculated_f = calc_pll_freqs(1, 1, counter, 1, 4, 4)/1000;
    if (calculated_f != frequency) {
        pc.printf("PLL frequency not match!\r\n");
    }
  } */ 
  else if (frequency >= 325) {
    //counter between 65 and 125
    // 20*2*65/8=325
    // 20*2*125/8=625
    jtag_pll(jtag, 1, 2, counter, 1, 8, 8);
  } else if (frequency >= 210) {
    //counter between 42 and 65 
    // 20*3*42/12=210
    // 20*3*65/12=325
    jtag_pll(jtag, 1, 3, counter, 1, 12, 12);
  } else if (frequency >= 105) {
    //counter between 21 and 42 
    // 20*6*21/24=125
    // 20*6*42/24=210
    jtag_pll(jtag, 1, 3, 2*counter, 1, 24, 24);
  } else {
    pc.printf("Frequency < 105M out of range!\r\nClock source changed to HCLK_EXT\r\n");
    return 1;
  }
  jtag.writeMemory(PLL_RESET, 1);
  wait_us(10);
  jtag.writeMemory(PLL_RESET, 0);
  wait_us(10);
  if(jtag.readMemory(PLL_PLLLOCK)==0) {
    pc.printf("PLL lock failed!\r\nClock source changed to HCLK_EXT\r\n");
    return 1;
  } else {
    jtag.writeMemory(intclk_source, 2);
    return 0;
  }
}