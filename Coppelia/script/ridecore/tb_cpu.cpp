// Copyright (c) Institute of Computing Technology, Chinese Academy of Sciences
//
// This source code is patent protected and being made available under the
// terms explained in the ../LICENSE-Academic and ../LICENSE-GOV files.
//
// Author: Yufeng Li
// Email: crazybinary494@gmail.com

#include <klee/klee.h>
#include <iostream>
#include <typeinfo>
#include "obj_dir/Vtopsim.h"
#include "obj_dir/Vtopsim__Syms.h"
#include "verilated.h"
#include "verilated_vcd_c.h"
#include "../../include/ridecore_constraints.h"

vluint32_t main_time = 0; // initial the simulation time

double sc_time_stamp(){
  return main_time;
}

int main(int argc, char** argv) {
  Verilated::commandArgs(argc, argv); // check arguments
  // simulation
  //Verilated::traceEverOn(true);

  Vtopsim* top = new Vtopsim; // create model
  // simulation
  //VerilatedVcdC* tfp = new VerilatedVcdC;

  // simulation
  //top->trace(tfp, 0);
  //tfp->open("./obj_dir/wave.vcd");

  vluint8_t clk, rst_n, qed_exec_dup;
  // set initial state to reset 
  rst_n = 0; // neg-edge reset
  top->reset_x = rst_n;
  clk = 0;
  qed_exec_dup = 1;
  #define RESET_CYCLES 5 

  printf("Begin reset ...\n");

  while(sc_time_stamp() < RESET_CYCLES) {
    clk = !clk;
    top->clk = clk; // the rising edge of the clock
    top->eval();

    clk = !clk;
    top->clk = clk; // the falling edge of the clock
    top->eval();

    // simulation
    //tfp->dump(sc_time_stamp());
    main_time++;
  }

  printf("Simulate %d cycles\n", main_time);
  printf("End reset\n");

  // 0x02400093 addi r1 r0 imm(36)
  // 0x02400893 addi r17 r0 imm(36)
  // 0x021095b3 mulh r11 r1 r1
  // 0x03189db3 mulh r27 r17 r17
  // 0x00f7a433 slt r8 r15 r15 
  // 0x00000013 addi r0 r0 0 (nop)
  
  // simulation
  //vluint32_t instr[5] = {0x02400093, 0x02400893, 0x021095b3, 0x03189db3, 0x00f7a433};
  #define NUM_INSTR 5
  vluint32_t sym_instr[NUM_INSTR];
  uint32_t gpr1, gpr2, gpr3, gpr4, gpr5, gpr6, gpr7, gpr8, gpr9, gpr10,
           gpr11, gpr12, gpr13, gpr14, gpr15, gpr16, gpr17, gpr18, gpr19, gpr20,
           gpr21, gpr22, gpr23, gpr24, gpr25, gpr26, gpr27, gpr28, gpr29, gpr30, gpr31;

  klee_make_symbolic(&sym_instr, sizeof(sym_instr), "sym_instr");
  
  klee_make_symbolic(&gpr1, sizeof(gpr1), "gpr1");
  klee_make_symbolic(&gpr2, sizeof(gpr2), "gpr2");
  klee_make_symbolic(&gpr3, sizeof(gpr3), "gpr3");
  klee_make_symbolic(&gpr4, sizeof(gpr4), "gpr4");
  klee_make_symbolic(&gpr5, sizeof(gpr5), "gpr5");
  klee_make_symbolic(&gpr6, sizeof(gpr6), "gpr6");
  klee_make_symbolic(&gpr7, sizeof(gpr7), "gpr7");
  klee_make_symbolic(&gpr8, sizeof(gpr8), "gpr8");
  klee_make_symbolic(&gpr9, sizeof(gpr9), "gpr9");
  klee_make_symbolic(&gpr10, sizeof(gpr10), "gpr10");
  klee_make_symbolic(&gpr11, sizeof(gpr11), "gpr11");
  klee_make_symbolic(&gpr12, sizeof(gpr12), "gpr12");
  klee_make_symbolic(&gpr13, sizeof(gpr13), "gpr13");
  klee_make_symbolic(&gpr14, sizeof(gpr14), "gpr14");
  klee_make_symbolic(&gpr15, sizeof(gpr15), "gpr15");
  klee_make_symbolic(&gpr16, sizeof(gpr16), "gpr16");
  klee_make_symbolic(&gpr17, sizeof(gpr17), "gpr17");
  klee_make_symbolic(&gpr18, sizeof(gpr18), "gpr18");
  klee_make_symbolic(&gpr19, sizeof(gpr19), "gpr19");
  klee_make_symbolic(&gpr20, sizeof(gpr20), "gpr20");
  klee_make_symbolic(&gpr21, sizeof(gpr21), "gpr21");
  klee_make_symbolic(&gpr22, sizeof(gpr22), "gpr22");
  klee_make_symbolic(&gpr23, sizeof(gpr23), "gpr23");
  klee_make_symbolic(&gpr24, sizeof(gpr24), "gpr24");
  klee_make_symbolic(&gpr25, sizeof(gpr25), "gpr25");
  klee_make_symbolic(&gpr26, sizeof(gpr26), "gpr26");
  klee_make_symbolic(&gpr27, sizeof(gpr27), "gpr27");
  klee_make_symbolic(&gpr28, sizeof(gpr28), "gpr28");
  klee_make_symbolic(&gpr29, sizeof(gpr29), "gpr29");
  klee_make_symbolic(&gpr30, sizeof(gpr30), "gpr30");
  klee_make_symbolic(&gpr31, sizeof(gpr31), "gpr31");

  // constraint instructions comply with RV32 ISA specification
  for(vluint8_t i = 0; i < NUM_INSTR; i++){
    rv_constraints(sym_instr[i]);
  }

  // constraint initial states are qed-consistent states
  klee_assume(gpr1 == gpr17 & gpr2 == gpr18 & gpr3 == gpr19 & gpr4 == gpr20 &
              gpr5 == gpr21 & gpr6 == gpr22 & gpr7 == gpr23 & gpr8 == gpr24 &
              gpr9 == gpr25 & gpr10 == gpr26 & gpr11 == gpr27 & gpr12 == gpr28 &
              gpr13 == gpr29 & gpr14 == gpr30 & gpr15 == gpr31);

  top->__VlSymsp->TOP__topsim__pipe__aregfile__regfile.__PVT__mem[1] = gpr1;
  top->__VlSymsp->TOP__topsim__pipe__aregfile__regfile.__PVT__mem[2] = gpr2;
  top->__VlSymsp->TOP__topsim__pipe__aregfile__regfile.__PVT__mem[3] = gpr3;
  top->__VlSymsp->TOP__topsim__pipe__aregfile__regfile.__PVT__mem[4] = gpr4;
  top->__VlSymsp->TOP__topsim__pipe__aregfile__regfile.__PVT__mem[5] = gpr5;
  top->__VlSymsp->TOP__topsim__pipe__aregfile__regfile.__PVT__mem[6] = gpr6;
  top->__VlSymsp->TOP__topsim__pipe__aregfile__regfile.__PVT__mem[7] = gpr7;
  top->__VlSymsp->TOP__topsim__pipe__aregfile__regfile.__PVT__mem[8] = gpr8;
  top->__VlSymsp->TOP__topsim__pipe__aregfile__regfile.__PVT__mem[9] = gpr9;
  top->__VlSymsp->TOP__topsim__pipe__aregfile__regfile.__PVT__mem[10] = gpr10;
  top->__VlSymsp->TOP__topsim__pipe__aregfile__regfile.__PVT__mem[11] = gpr11;
  top->__VlSymsp->TOP__topsim__pipe__aregfile__regfile.__PVT__mem[12] = gpr12;
  top->__VlSymsp->TOP__topsim__pipe__aregfile__regfile.__PVT__mem[13] = gpr13;
  top->__VlSymsp->TOP__topsim__pipe__aregfile__regfile.__PVT__mem[14] = gpr14;
  top->__VlSymsp->TOP__topsim__pipe__aregfile__regfile.__PVT__mem[15] = gpr15;
  top->__VlSymsp->TOP__topsim__pipe__aregfile__regfile.__PVT__mem[16] = gpr16;
  top->__VlSymsp->TOP__topsim__pipe__aregfile__regfile.__PVT__mem[17] = gpr17;
  top->__VlSymsp->TOP__topsim__pipe__aregfile__regfile.__PVT__mem[18] = gpr18;
  top->__VlSymsp->TOP__topsim__pipe__aregfile__regfile.__PVT__mem[19] = gpr19;
  top->__VlSymsp->TOP__topsim__pipe__aregfile__regfile.__PVT__mem[20] = gpr20;
  top->__VlSymsp->TOP__topsim__pipe__aregfile__regfile.__PVT__mem[21] = gpr21;
  top->__VlSymsp->TOP__topsim__pipe__aregfile__regfile.__PVT__mem[22] = gpr22;
  top->__VlSymsp->TOP__topsim__pipe__aregfile__regfile.__PVT__mem[23] = gpr23;
  top->__VlSymsp->TOP__topsim__pipe__aregfile__regfile.__PVT__mem[24] = gpr24;
  top->__VlSymsp->TOP__topsim__pipe__aregfile__regfile.__PVT__mem[25] = gpr25;
  top->__VlSymsp->TOP__topsim__pipe__aregfile__regfile.__PVT__mem[26] = gpr26;
  top->__VlSymsp->TOP__topsim__pipe__aregfile__regfile.__PVT__mem[27] = gpr27;
  top->__VlSymsp->TOP__topsim__pipe__aregfile__regfile.__PVT__mem[28] = gpr28;
  top->__VlSymsp->TOP__topsim__pipe__aregfile__regfile.__PVT__mem[29] = gpr29;
  top->__VlSymsp->TOP__topsim__pipe__aregfile__regfile.__PVT__mem[30] = gpr30;
  top->__VlSymsp->TOP__topsim__pipe__aregfile__regfile.__PVT__mem[31] = gpr31;

  printf("Begin symbolic simulation ...\n");

  rst_n = 1; // unset reset during symbolic simulation
  top->reset_x = rst_n;
  #define RUN_CYCLES 50
  
  while(sc_time_stamp() < RUN_CYCLES) {
    clk = !clk;
    qed_exec_dup = !qed_exec_dup;
    top->clk = clk;
    top->qed_exec_dup = qed_exec_dup;
    // if(main_time >= 7 & main_time <= 11)
    //   top->instruction = instr[main_time - 7];
    if(main_time >= 7 & main_time <= 11)
      top->instruction = sym_instr[main_time - 7];
    else
      top->instruction = 0x00000013;
    top->eval();

    clk = !clk;
    top->clk = clk;
    top->eval();

    printf("%d,%d", top->__VlSymsp->TOP__topsim__pipe.__PVT__num_orig_insts, top->__VlSymsp->TOP__topsim__pipe.__PVT__num_dup_insts);
    printf(" QED-ready is %d\n", top->__VlSymsp->TOP__topsim__pipe.__PVT__qed_ready);

    if(top->__VlSymsp->TOP__topsim__pipe.__PVT__qed_ready) {
      klee_assert(top->__VlSymsp->TOP__topsim__pipe__aregfile.__PVT__mem11 == top->__VlSymsp->TOP__topsim__pipe__aregfile.__PVT__mem27);
      //assert(top->__VlSymsp->TOP__topsim__pipe__aregfile.__PVT__mem11 == top->__VlSymsp->TOP__topsim__pipe__aregfile.__PVT__mem27);
    };

    // if(top->__VlSymsp->TOP__topsim__pipe.__PVT__qed_ready){
    //   printf("QED ready!\n");
    //   klee_assert(0);
    // }

    // simulation
    //tfp->dump(sc_time_stamp());
    main_time++;
    printf("Runing for %d cycles\n", main_time);
  }

  top->final(); // done simulation
  // simulation
  //tfp->close();
  //delete tfp;
  delete top;

  exit(0);
}
