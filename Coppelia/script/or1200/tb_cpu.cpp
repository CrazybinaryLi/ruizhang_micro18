#include "obj_dir/Vor1200_cpu.h"
#include "obj_dir/Vor1200_cpu__Syms.h"
#include "verilated.h"
#include <klee/klee.h>
#include "../../include/or1k_constraints.h"
int main(int argc, char **argv) {
   Vor1200_cpu* top = new Vor1200_cpu;
   int clk, rst;

   rst = 1; 
   top->rst = rst; 
   clk = 1; 
   clk = !clk; 
   top->clk = clk; 
   top->eval(); 
   clk = !clk; 
   top->clk = clk; 
   top->eval(); 
   clk = !clk; 
   top->clk = clk; 
   top->eval(); 
   clk = !clk; 
   top->clk = clk; 
   top->eval(); 
unsigned int icpu_dat_i, icpu_ack_i, icpu_rty_i, icpu_err_i;
unsigned int icpu_adr_i, icpu_tag_i, dcpu_dat_i, dcpu_ack_i;
unsigned int dcpu_rty_i, dcpu_err_i, dcpu_tag_i, boot_adr_sel_i;
unsigned int mtspr_dc_done, sig_int, sig_tick;

unsigned int delayed1_ex_dslot, ex_dslot, wb_pc, delayed_iee;
unsigned int delayed_tee, state, except_type, ex_freeze_prev;
unsigned int trace_trap, sr_reg_bit_eph_select, mul_stall_count, wait_lsu;
unsigned int pcreg_select, multicycle_cnt, div_free, div_cntr;
unsigned int div_quot_r, waiting_on, saved_a, saved_b, sp_return_counter;
unsigned int r1, r2;

klee_make_symbolic(&icpu_dat_i, sizeof(icpu_dat_i), "icpu_dat_i");
klee_make_symbolic(&icpu_ack_i, sizeof(icpu_ack_i), "icpu_ack_i");
klee_make_symbolic(&icpu_rty_i, sizeof(icpu_rty_i), "icpu_rty_i");
klee_make_symbolic(&icpu_adr_i, sizeof(icpu_adr_i), "icpu_adr_i");
klee_make_symbolic(&icpu_tag_i, sizeof(icpu_tag_i), "icpu_tag_i");
klee_make_symbolic(&dcpu_dat_i, sizeof(dcpu_dat_i), "dcpu_dat_i");
klee_make_symbolic(&dcpu_ack_i, sizeof(dcpu_ack_i), "dcpu_ack_i");
klee_make_symbolic(&dcpu_rty_i, sizeof(dcpu_rty_i), "dcpu_rty_i");
klee_make_symbolic(&dcpu_tag_i, sizeof(dcpu_tag_i), "dcpu_tag_i");
klee_make_symbolic(&boot_adr_sel_i, sizeof(boot_adr_sel_i), "boot_adr_sel_i");
klee_make_symbolic(&mtspr_dc_done, sizeof(mtspr_dc_done), "mtspr_dc_done");
klee_make_symbolic(&sig_int, sizeof(sig_int), "sig_int");
klee_make_symbolic(&sig_tick, sizeof(sig_tick), "sig_tick");
klee_make_symbolic(&r1, sizeof(r1), "r1");
klee_make_symbolic(&r2, sizeof(r2), "r2");
klee_make_symbolic(&pcreg_select, sizeof(pcreg_select), "pcreg_select");
klee_make_symbolic(&multicycle_cnt, sizeof(multicycle_cnt), "multicycle_cnt");
klee_make_symbolic(&waiting_on, sizeof(waiting_on), "waiting_on");

or1k_constraints(icpu_dat_i);
klee_assume((icpu_ack_i <= 1) & (icpu_rty_i <= 1) & 
(icpu_tag_i <= 7) & 
(dcpu_ack_i <= 1) & (dcpu_rty_i <= 1) & 
(dcpu_tag_i <= 7) & 
(boot_adr_sel_i <= 1) & (mtspr_dc_done <= 1) & 
(sig_int <= 1) & (sig_tick <= 1)
& (pcreg_select <= 1) 
& (multicycle_cnt <= 7) 
& (waiting_on <= 3) 
); 

top->icpu_dat_i = icpu_dat_i;
top->icpu_ack_i = icpu_ack_i;
top->icpu_rty_i = icpu_rty_i;
top->icpu_err_i = 0;
top->icpu_adr_i = icpu_adr_i;
top->icpu_tag_i = icpu_tag_i;
top->dcpu_dat_i = dcpu_dat_i;
top->dcpu_ack_i = dcpu_ack_i;
top->dcpu_rty_i = dcpu_rty_i;
top->dcpu_err_i = 0;
top->dcpu_tag_i = dcpu_tag_i;
top->boot_adr_sel_i = boot_adr_sel_i;
top->mtspr_dc_done = mtspr_dc_done;
top->sig_int = sig_int;
top->sig_tick = sig_tick;
top->du_stall = 0;
top->du_addr = 0;
top->du_dat_du = 0;
top->du_read = 0;
top->du_write = 0;
top->du_dsr = 0;
top->du_dmr1 = 0;
top->du_hwbkpt = 0;
top->du_hwbkpt_ls_r = 0;
top->du_flush_pipe = 0;
top->spr_dat_pic = 0;
top->spr_dat_tt = 0;
top->spr_dat_pm = 0;
top->spr_dat_dmmu = 0;
top->spr_dat_immu = 0;
top->spr_dat_du = 0;

top->__VlSymsp->TOP__or1200_cpu.__PVT__or1200_genpc__DOT__pcreg_select = pcreg_select;
top->__VlSymsp->TOP__or1200_cpu.__PVT__or1200_freeze__DOT__multicycle_cnt = multicycle_cnt;
top->__VlSymsp->TOP__or1200_cpu.__PVT__or1200_freeze__DOT__waiting_on = waiting_on;
top->__VlSymsp->TOP__or1200_cpu__or1200_rf__rf_a.__PVT__mem[16] = r1;
top->__VlSymsp->TOP__or1200_cpu__or1200_rf__rf_b.__PVT__mem[17] = r2;

   rst = 0;
   clk = 1;
   top->rst = rst;

   clk = !clk;
   top->clk = clk;
   top->eval();

   clk = !clk;
   top->clk = clk;
   top->eval();

   clk = !clk;
   top->clk = clk;
   top->eval();

   clk = !clk;
   top->clk = clk;
   top->eval();

if (((top->__VlSymsp->TOP__or1200_cpu__or1200_ctrl.ex_insn & 4292870144) >> 21 == 1826) && 
   (top->__VlSymsp->TOP__or1200_cpu.__PVT__operand_a > top->__VlSymsp->TOP__or1200_cpu.__PVT__operand_b) && 
   (((top->__VlSymsp->TOP__or1200_cpu__or1200_sprs.__PVT__to_sr >> 9) & 1) != 1)) 
   klee_assert(0);

   delete top;
   exit(0);
}
