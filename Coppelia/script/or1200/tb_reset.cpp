#include "obj_dir/Vor1200_cpu.h"
#include "obj_dir/Vor1200_cpu__Syms.h"
#include "verilated.h"
#include <iostream>
#include <typeinfo>
int main(int argc, char **argv) {
   Vor1200_cpu* top = new Vor1200_cpu;
   int clk, rst;

   rst = 1;
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

   std::cout << (int)top->__VlSymsp->TOP__or1200_cpu.__PVT__or1200_genpc__DOT__pcreg_select << " ";
   std::cout << (int)top->__VlSymsp->TOP__or1200_cpu.__PVT__or1200_freeze__DOT__multicycle_cnt << " ";
   std::cout << (int)top->__VlSymsp->TOP__or1200_cpu.__PVT__or1200_freeze__DOT__waiting_on << " ";
   std::cout << (int)top->__VlSymsp->TOP__or1200_cpu__or1200_rf__rf_a.__PVT__mem[16] << " ";
   std::cout << (int)top->__VlSymsp->TOP__or1200_cpu__or1200_rf__rf_b.__PVT__mem[17] << " ";

   delete top;
   exit(0);
}
