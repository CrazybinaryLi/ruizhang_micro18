# Coppelia Reproduction

## Description
This repository reproduces the Coppelia tool described in paper [MICRO'18](https://cs.unc.edu/~rzhang/files/MICRO2018.pdf). The original Coppelia repository can be found at [https://github.com/rzhang2285/Coppelia.git](https://github.com/rzhang2285/Coppelia.git); however, the steps outlined are somewhat brief. As many software versions have evolved, the original Coppelia is no longer directly compatible with newer operating systems.

## Directory Tree
- ruizhang_micro18
    - Coppelia: Coppelia source code are obtained from [GitHub](https://github.com/rzhang2285/Coppelia.git).    
        - cores: RTL code of Design Under Test 
            - biriscv: 32-bit superscalar RISC-V core, which is obtained from [GitHub](https://github.com/ultraembedded/biriscv)
            - or1200: OpenRISC 1200 implementation
            - ridecore: RISC-V out-of-order core
        - include: Header files for constraining symbolic instructions to comply with ISA specifications   
        - klee: Modified KLEE for Coppelia
        - LICENSE.txt
        - one_cycle
        - README.md
        - script
        - verilator
            - verilated.mk: Verilator' Makefile for Coppelia 
    - Dockerfile: Docker image build file for this experiment
    - gtest-1.7.0.zip: Google test sources
    - klee: klee v1.4.0 is obtained from [GitHub](https://github.com/klee/klee.git)
    - klee-tutorial: klee tutorial materials from https://klee-se.org/releases/docs/v1.3.0/tutorials/ 
    - klee-uclibc: uclibc and POSIX sources
    - llvm-clang: LLVM 3.4 sources
    - minisat: minisat solver
    - README.md
    - release-1.7.0.zip: 
    - stp: STP solver
    - verilator: verilator v3.900 is obtained from [GitHub](http://git.veripool.org/git/verilator)
    - whole-program-llvm: A wrapper script to build whole-program LLVM bitcode files from [GitHub](https://github.com/travitch/whole-program-llvm)