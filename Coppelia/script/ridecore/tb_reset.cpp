#include "obj_dir/Vtopsim.h"
#include "obj_dir/Vtopsim__Syms.h"
#include "verilated.h"
#include <iostream>
#include <typeinfo>

int main(int argc, char **argv){
    // Initialize Verilators variables
    Verilated::commandArgs(argc, argv);
    
    // Create an instance of our module under test
    Vtopsim* top = new Vtopsim;
    int clk, rst_n;

    rst_n = 0;
    clk = 1;
    top->reset_x = rst_n;

    // Cycle 1
    clk = !clk;
    top -> clk = clk;
    top -> eval();

    clk = !clk;
    top -> clk = clk;
    top -> eval();

    // Cycle 2
    clk = !clk;
    top -> clk = clk;
    top -> eval();

    clk = !clk;
    top -> clk = clk;
    top -> eval();

    // Cycle 3
    clk = !clk;
    top -> clk = clk;
    top -> eval();

    clk = !clk;
    top -> clk = clk;
    top -> eval();

    // Cycle 4
    clk = !clk;
    top -> clk = clk;
    top -> eval();

    clk = !clk;
    top -> clk = clk;
    top -> eval();

    // Cycle 5
    clk = !clk;
    top -> clk = clk;
    top -> eval();

    clk = !clk;
    top -> clk = clk;
    top -> eval();

    std::cout << (int)top->__VlSymsp->TOP__topsim__pipe__aregfile.__PVT__mem1 << " ";
    std::cout << (int)top->__VlSymsp->TOP__topsim__pipe__aregfile.__PVT__mem17 << " ";

    top -> final();
    delete top;
    exit(0);
}