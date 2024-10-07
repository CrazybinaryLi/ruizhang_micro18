// Copyright (c) Institute of Computing Technology, Chinese Academy of Sciences
//
// This source code can only be used for academic purposes!
//
// Author: Yufeng Li
// Email: crazybinary494@gmail.com

#include <klee/klee.h>
#include <iostream>
#include <typeinfo>
#include "obj_dir/Vtb_top.h"
#include "obj_dir/Vtb_top__Syms.h"
#include "verilated.h"
#include "verilated_vcd_c.h"
#include "../../include/biriscv_constraints.h"

vluint64_t main_time = 0; // initial the simulation time

double sc_time_stamp(){
  return main_time;
}

int main(int argc, char** argv) {
  //Verilated::commandArgs(argc, argv); // check arguments
  //Verilated::traceEverOn(true);

  Vtb_top* top = new Vtb_top; // create model
  //VerilatedVcdC* tfp = new VerilatedVcdC;

  //top->trace(tfp, 0);
  //tfp->open("./obj_dir/wave.vcd");

  vluint8_t clk, rst, qed_exec_dup;
  // set initial state to reset 
  rst = 1; // pos-edge reset
  top->rst = rst;
  qed_exec_dup = 0; // control signal of submitting duplicate instructions 
  top->qed_exec_dup = qed_exec_dup;
  clk = 0;
  #define RESET_CYCLES 5

  printf("Begin reset ...\n");

  while(sc_time_stamp() < RESET_CYCLES) {
    clk = !clk;     // clk = 1
    top->clk = clk; // the rising edge of the clock
    top->eval();

    clk = !clk;     // clk = 0
    top->clk = clk; // the falling edge of the clock
    top->eval();

    //tfp->dump(sc_time_stamp());
    main_time++;
  }

  printf("Simulate %d cycles\n", main_time);
  printf("End reset\n");

  uint32_t instruction;
  uint32_t gpr1, gpr2, gpr3, gpr4, gpr5, gpr6, gpr7, gpr8, gpr9, gpr10,
           gpr11, gpr12, gpr13, gpr14, gpr15, gpr16, gpr17, gpr18, gpr19, gpr20,
           gpr21, gpr22, gpr23, gpr24, gpr25, gpr26, gpr27, gpr28, gpr29, gpr30, gpr31;

  printf("Begin symbolic simulation ...\n");

  klee_make_symbolic(&instruction, sizeof(instruction), "instruction");
  
  klee_make_symbolic(&gpr1, sizeof(gpr1), "gpr1");
  // klee_make_symbolic(&gpr2, sizeof(gpr2), "gpr2");
  // klee_make_symbolic(&gpr3, sizeof(gpr3), "gpr3");
  // klee_make_symbolic(&gpr4, sizeof(gpr4), "gpr4");
  // klee_make_symbolic(&gpr5, sizeof(gpr5), "gpr5");
  // klee_make_symbolic(&gpr6, sizeof(gpr6), "gpr6");
  // klee_make_symbolic(&gpr7, sizeof(gpr7), "gpr7");
  // klee_make_symbolic(&gpr8, sizeof(gpr8), "gpr8");
  // klee_make_symbolic(&gpr9, sizeof(gpr9), "gpr9");
  // klee_make_symbolic(&gpr10, sizeof(gpr10), "gpr10");
  // klee_make_symbolic(&gpr11, sizeof(gpr11), "gpr11");
  // klee_make_symbolic(&gpr12, sizeof(gpr12), "gpr12");
  // klee_make_symbolic(&gpr13, sizeof(gpr13), "gpr13");
  // klee_make_symbolic(&gpr14, sizeof(gpr14), "gpr14");
  // klee_make_symbolic(&gpr15, sizeof(gpr15), "gpr15");
  // klee_make_symbolic(&gpr16, sizeof(gpr16), "gpr16");
  klee_make_symbolic(&gpr17, sizeof(gpr17), "gpr17");
  // klee_make_symbolic(&gpr18, sizeof(gpr18), "gpr18");
  // klee_make_symbolic(&gpr19, sizeof(gpr19), "gpr19");
  // klee_make_symbolic(&gpr20, sizeof(gpr20), "gpr20");
  // klee_make_symbolic(&gpr21, sizeof(gpr21), "gpr21");
  // klee_make_symbolic(&gpr22, sizeof(gpr22), "gpr22");
  // klee_make_symbolic(&gpr23, sizeof(gpr23), "gpr23");
  // klee_make_symbolic(&gpr24, sizeof(gpr24), "gpr24");
  // klee_make_symbolic(&gpr25, sizeof(gpr25), "gpr25");
  // klee_make_symbolic(&gpr26, sizeof(gpr26), "gpr26");
  // klee_make_symbolic(&gpr27, sizeof(gpr27), "gpr27");
  // klee_make_symbolic(&gpr28, sizeof(gpr28), "gpr28");
  // klee_make_symbolic(&gpr29, sizeof(gpr29), "gpr29");
  // klee_make_symbolic(&gpr30, sizeof(gpr30), "gpr30");
  // klee_make_symbolic(&gpr31, sizeof(gpr31), "gpr31");

  // constraint instructions comply with RV32 ISA specification
  rv_constraints(instruction);
  top->instruction = instruction;

  // debug
  //top->instruction = 0b00000010000110001001000010110011; //0x021890b3(mulh r1, r17, r1)

  // constraint initial states are qed-consistent states
  klee_assume(gpr1 == gpr17);
  
  top->__VlSymsp->TOP__tb_top__u_dut__u_issue__u_regfile.__PVT__genblk1__DOT__REGFILE__DOT__reg_r1_q = gpr1;
  top->__VlSymsp->TOP__tb_top__u_dut__u_issue__u_regfile.__PVT__genblk1__DOT__REGFILE__DOT__reg_r17_q = gpr17;
  
  rst = 0; // unset reset during symbolic simulation
  top->rst = rst; 
  #define RUN_CYCLES 31

  while(sc_time_stamp() < RUN_CYCLES & !Verilated::gotFinish()) {
    clk = !clk;
    qed_exec_dup = !qed_exec_dup;
    top->clk = clk;
    top->qed_exec_dup = qed_exec_dup;
    top->eval();

    clk = !clk;
    top->clk = clk;
    top->eval();

    printf("%d,%d\n", 
          top->__VlSymsp->TOP__tb_top__u_dut__u_issue__prop.__PVT__num_orig_insts, 
          top->__VlSymsp->TOP__tb_top__u_dut__u_issue__prop.__PVT__num_dup_insts);

    // if(top->__VlSymsp->TOP__tb_top__u_dut__u_issue__prop.__PVT__num_orig_insts == top->__VlSymsp->TOP__tb_top__u_dut__u_issue__prop.__PVT__num_dup_insts) {
    //   klee_assert(top->__VlSymsp->TOP__tb_top__u_dut__u_issue__prop.__PVT__r1 == 
    //     top->__VlSymsp->TOP__tb_top__u_dut__u_issue__prop.__PVT__r17);
    // }

    // if(top->__VlSymsp->TOP__tb_top__u_dut__u_issue__prop.__PVT__num_orig_insts == top->__VlSymsp->TOP__tb_top__u_dut__u_issue__prop.__PVT__num_dup_insts){
    //   printf("End symbolic simulation\n");
    //   klee_assert(0);
    // }
    
    //tfp->dump(sc_time_stamp());
    main_time++;
  }
 
  top->final(); // done simulation
  //tfp->close();
  //delete tfp;
  delete top;

  exit(0);
}
