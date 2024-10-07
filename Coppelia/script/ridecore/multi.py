#!/usr/bin/env python

import os
import sys
import glob

# prepare for klee 
os.system('ulimit -s unlimited')

# clean directory
# os.system('rm -rf obj_dir')

# os.system('verilator -DVERILATOR=1 -O0 -Wall --top-module topsim +incdir+../../cores/ridecore --Mdir obj_dir --cc ../../cores/ridecore/*.v --exe tb_reset.cpp')
# os.system('make -j -C obj_dir/ -f Vtopsim.mk Vtopsim')
# os.system('obj_dir/Vtopsim > rstValues.txt')

# clean directory
os.system('rm -rf obj_dir')

os.system('verilator -DVERILATOR=1 -O0 -Wall --trace --top-module topsim +incdir+../../cores/ridecore -Mdir obj_dir -cc ../../cores/ridecore/*.v --exe tb_cpu.cpp')
os.system('make -j -C obj_dir/ -f Vtopsim.mk Vtopsim')
os.system('extract-bc -l llvm-link obj_dir/Vtopsim')

# modified klee version
os.system('coppelia-klee -coi-prune=true -fast-validation=true --search=hardware -halt-when-fired=true -emit-all-errors -max-memory=100000000 --libc=uclibc --posix-runtime obj_dir/Vtopsim.bc')

# original klee version
#os.system('klee -emit-all-errors --search=dfs -max-memory=100000000 --libc=uclibc --posix-runtime obj_dir/Vtopsim.bc')
#os.system('klee -emit-all-errors --search=random-state -max-memory=100000000 --libc=uclibc --posix-runtime obj_dir/Vtopsim.bc')