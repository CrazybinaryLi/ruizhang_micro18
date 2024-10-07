#!/usr/bin/env python

import os
import sys
import glob

# prepare for klee 
os.system('ulimit -s unlimited')

# clean directory
# os.system('rm -rf obj_dir')

# os.system('verilator -DVERILATOR=1 -O0 -Wall --top-module tb_top +incdir+../../cores/biriscv --Mdir obj_dir --cc ../../cores/biriscv/*.v --exe tb_reset.cpp')
# os.system('make -j -C obj_dir/ -f Vtb_top.mk Vtb_top')
# os.system('obj_dir/Vtb_top > rstValues.txt')

# clean directory
os.system('rm -rf obj_dir')

os.system('verilator --trace -DVERILATOR=1 -O0 -Wall --top-module tb_top +incdir+../../cores/biriscv -Mdir obj_dir -cc ../../cores/biriscv/*.v --exe tb_cpu.cpp')
os.system('make -j -C obj_dir/ -f Vtb_top.mk Vtb_top')
os.system('extract-bc -l llvm-link obj_dir/Vtb_top')
# modified klee version
#os.system('coppelia-klee -coi-prune=true -fast-validation=true --search=hardware -halt-when-fired=true -emit-all-errors -max-memory=100000000 --libc=uclibc --posix-runtime obj_dir/Vtb_top.bc')
# original klee version
os.system('klee -emit-all-errors --search=random-state -max-memory=100000000 --libc=uclibc --posix-runtime obj_dir/Vtb_top.bc')