#ifndef MMAP_H_
#define MMAP_H_

// SRAM Limits
#define IMEM_MIN 0x00000000
#define IMEM_MAX 0x00010000
#define DMEM_MIN 0x20000000
#define DMEM_MAX 0x2002C000

// ram buffer for data transmission between mbed and cortex-m3
// read from IO_TYPE to check whether the request is print or scan request
#define WRITEBUF_BEGIN (0x24000100)
#define WRITEBUF_END (0x2400017C)
#define READBUF_BEGIN   (0x24000180)
#define READBUF_END     (0x240001FC)
#define BUF_SIZE ((WRITEBUF_END - WRITEBUF_BEGIN)*4)
#define IO_TYPE    (0x2400017C)
#define ACK_TYPE    (0x240001FC)
// Redefine Imem addr from 0x10000000 to 0x00000000
#define set_imem 0x44000008

// GPIO Memory Mapped IO
// R/W/Set all at once
#define GPIO_VAL 0x44001000
#define GPIO_DIR 0x44001004
#define GPIO_MASK 0x4400108
#define GPIO_INT 0x4400100c
// R/W/Set individually start addresses
#define GPIO_VAL_AR 0x4400_1100
#define GPIO_DIR_AR 0x4400_1200
#define GPIO_INT_AR 0x4400_1300

// Counter
#define reset_the_count 0x44010010
#define the_count_31_0 0x44010000
#define the_count_63_32 0x44010004
#define the_thresh_31_0 0x44010008
#define the_thresh_63_32 0x4401000c
#define en_count 0x44010014

// Not used
#define use_scan 0x44100000

// Ring oscillator clock
#define en_ring_osc 0x44100004
#define ring_osc_reset 0x44100008
#define ring_osc_clksel 0x4410000c

// Clock selection
#define intclk_source 0x44100010
#define int_div_by 0x44100014///////////////////////////////////////
#define ext_div_by 0x44100018///////////////////////////////////////
#define extclk_source 0x4410001c

/*
   always @*
   begin
      case (intclk_source)
      2'b00 : HCLK = HCLK_EXT;
      2'b01 : HCLK = ring_osc_HCLK;
      2'b10 : HCLK = PLL_PLLOUTA;
      2'b11 : HCLK = PLL_PLLOUTB;
      default : HCLK = HCLK_EXT;
      endcase
   end



   always @*
   begin
      case (extclk_source)
      2'b00 : HCLK_div_down = HCLK_EXT;
      2'b01 : HCLK_div_down = HCLK_divider;
      2'b10 : HCLK_div_down = PLL_PLLOUTA;
      2'b11 : HCLK_div_down = PLL_PLLOUTB;
      default : HCLK_div_down = HCLK_EXT;
      endcase
   end
   
      always @*
   begin
      case (ext_div_by)
4'd0 : HCLK_divider = HCLK_div_2;
4'd1 : HCLK_divider = HCLK_div_2;
4'd2 : HCLK_divider = HCLK_div_4;
4'd3 : HCLK_divider = HCLK_div_8;
4'd4 : HCLK_divider = HCLK_div_16;
4'd5 : HCLK_divider = HCLK_div_32;
4'd6 : HCLK_divider = HCLK_div_64;
4'd7 : HCLK_divider = HCLK_div_128;
4'd8 : HCLK_divider = HCLK_div_256;
4'd9 : HCLK_divider = HCLK_div_512;
4'd10 : HCLK_divider = HCLK_div_1024;
4'd11 : HCLK_divider = HCLK_div_2048;
4'd12 : HCLK_divider = HCLK_div_4096;
4'd13 : HCLK_divider = HCLK_div_8192;
4'd14 : HCLK_divider = HCLK_div_16384;
4'd15 : HCLK_divider = HCLK_div_32768;
      default : HCLK_divider = HCLK_div_2;
      endcase
   end

*/

// Resets
#define reset_reg 0x44000004
#define RESET_scan 0x44100020
#define CORERESET_scan 0x44100024

// DDRO
#define ddro_syn_en 0x44100028
#define ddro_inv_en 0x4410002c
#define ddro_ref_src 0x44100030
#define ddro_samp_src 0x44100034
#define ddro_start 0x44100038
#define ddro_threshold 0x4410003c
#define ddro_pad_out 0x44100040//////////////////////////////////////////
#define ddro_div_by 0x44100044///////////////////////////////////////////
#define ddro_done 0x44100060
#define ddro_count 0x44100064

/*
 reg          ref_clk;
   always @*
   begin
      case(ref_src)
      5'h0:  ref_clk = HCLK;
      5'h1:  ref_clk = ring_osc_HCLK;
      5'h2:  ref_clk = PLL_REFCLK;
      5'h3:  ref_clk = syn_out[0];
      5'h4:  ref_clk = syn_out[1];
      5'h5:  ref_clk = syn_out[2];
      5'h6:  ref_clk = syn_out[3];
      5'h7:  ref_clk = syn_out[4];
      5'h8:  ref_clk = syn_out[5];
      5'h9:  ref_clk = syn_out[6];
      5'ha:  ref_clk = syn_out[7];
      5'hb:  ref_clk = syn_out[8];
      5'hc:  ref_clk = inv_out[0];
      5'hd:  ref_clk = inv_out[1];
      5'he:  ref_clk = inv_out[2];
      5'hf:  ref_clk = inv_out[3];
      5'h10: ref_clk = inv_out[4];
      5'h11: ref_clk = inv_out[5];
      5'h12: ref_clk = inv_out[6];
      5'h13: ref_clk = inv_out[7];
      5'h14: ref_clk = inv_out[8];
      5'h15: ref_clk = inv_out[9];
      5'h16: ref_clk = OXIDE_CLK;
      5'h17: ref_clk = PMOS_CLK;
      5'h18: ref_clk = NMOS_CLK;
      5'h19: ref_clk = TEMP_CLK;
      default: ref_clk = HCLK;
      endcase
   end
*/

// Sensors
#define sensor_disable 0x44100048
#define sensor_oxide_stress 0x4410004c
#define sensor_reset 0x44100050
#define sensor_start 0x44100054
#define sensor_sel 0x44100058
#define sensor_done 0x44100068
#define sensor_bank_24_0 0x4410006c
#define sensor_bank_49_25 0x44100070
#define sensor_bank_74_50 0x44100074
#define sensor_bank_99_75 0x44100078

// ECC
#define ecc_mode 0x4410005c
#define imem_sec 0x4410007c
#define imem_ded 0x44100080
#define dmem_sec 0x44100084
#define dmem_ded 0x44100088

// PLL Settings
#define do_pll_reset 0x44020000
#define PLL_DCOBYPASS 0x4410008c
#define PLL_FFENABLE 0x44100090
#define PLL_FFSLEWRATE 0x44100094
#define PLL_FFTUNE 0x44100098
#define PLL_INTFBK 0x4410009c
#define PLL_LFTUNE_32_0 0x441000a0
#define PLL_LFTUNE_40_32 0x441000a4
#define PLL_LOCKSEL 0x441000a8
#define PLL_LOCKTUNE 0x441000ac
#define PLL_MULTFRAC 0x441000b0
#define PLL_MULTINT 0x441000b4
#define PLL_PLLBYPASS 0x441000b8
#define PLL_PREDIV 0x441000bc
#define PLL_RANGEA 0x441000c0
#define PLL_RANGEB 0x441000c4
#define PLL_RESET 0x441000c8
#define PLL_SDORDER 0x441000cc
#define PLL_SLEEP 0x441000d0
#define PLL_STOPCLKA 0x441000d4
#define PLL_STOPCLKB 0x441000d8
#define PLL_CE0CCSEFCG 0x441000dc
#define PLL_CE0CCTBON 0x441000e0
#define PLL_CE0MPGSE 0x441000e4
#define PLL_CE0TESTM3 0x441000e8
#define PLL_CE1CCB 0x441000ec
#define PLL_CE1MPGC1 0x441000f0
#define PLL_PLH 0x441000f4
#define PLL_TESTDIAG 0x441000f8
#define PLL_TESTSEL 0x441000fc
#define PLL_TSTCLKSEL 0x44100100
#define PLL_GPTRSI 0x44100104
#define PLL_DLT 0x44100108
#define PLL_VCITUNE 0x4410010c
#define PLL_VCVISEL 0x44100110
#define PLL_VCVTUNE 0x44100114
#define PLL_PLLLOCK 0x44100118
#define PLL_PLLSYNCA 0x4410011c
#define PLL_PLLSYNCB 0x44100120
#define PLL_GPTRSO 0x44100124
#define PLL_OBSERVE 0x44100128

// SRAM settings
#define imem_ARWY 0x4410012c
#define imem_DBWY 0x44100130
#define imem_MICLOCKMODE 0x44100134
#define imem_MIEMAS 0x44100138
#define imem_MIEMAW 0x4410013c
#define imem_MIEMAWASS 0x44100140
#define imem_MIFLOOD 0x44100144
#define imem_MIPGDISABLE 0x44100148
#define imem_MITESTM1 0x4410014c
#define imem_MITESTM3 0x44100150
#define imem_MIWASSD 0x44100154
#define imem_MIWRTM 0x44100158
#define imem_TAB 0x4410015c
#define imem_TAC 0x44100160
#define imem_TAD0 0x44100164
#define imem_TAW 0x44100168
#define imem_TBW 0x4410016c
#define imem_TD 0x44100170
#define imem_TDEEPSLEEP 0x44100174
#define imem_TQ 0x44100178
#define imem_TWRITE 0x4410017c
#define imem_TREAD 0x44100180
#define imem_CR 0x44100184
#define imem_CRE 0x44100188
#define imem_RR 0x4410018c
#define imem_RRE 0x44100190
#define dmem_ARWY 0x44100194
#define dmem_DBWY 0x44100198
#define dmem_MICLOCKMODE 0x4410019c
#define dmem_MIEMAS 0x441001a0
#define dmem_MIEMAW 0x441001a4
#define dmem_MIEMAWASS 0x441001a8
#define dmem_MIFLOOD 0x441001ac
#define dmem_MIPGDISABLE 0x441001b0
#define dmem_MITESTM1 0x441001b4
#define dmem_MITESTM3 0x441001b8
#define dmem_MIWASSD 0x441001bc
#define dmem_MIWRTM 0x441001c0
#define dmem_TAB 0x441001c4
#define dmem_TAC 0x441001c8
#define dmem_TAD0 0x441001cc
#define dmem_TAW 0x441001d0
#define dmem_TBW 0x441001d4
#define dmem_TD 0x441001d8
#define dmem_TDEEPSLEEP 0x441001dc
#define dmem_TQ 0x441001e0
#define dmem_TWRITE 0x441001e4
#define dmem_TREAD 0x441001e8
#define dmem_CR 0x441001ec
#define dmem_CRE 0x441001f0
#define dmem_RR 0x441001f4
#define dmem_RRE 0x441001f8

#endif