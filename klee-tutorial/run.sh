#!/bin/bash

for dir in klee-out*/; do
    if [ -d "$dir" ]; then
        echo "Removing $dir"
        rm -rf "$dir"
    fi
done


# Example 1: get_sign.c
# clang -emit-llvm -c -g get_sign.c

# -output-dir=path: Specifying a directory for outputs
# -max-time=seconds: Halt execution after the given number of seconds
# -max-forks=N: Stop forking after N symbolic branches, and run the remaining paths to termination
# -max-memory=N: Try to limit memory consumption to N megabytes

# klee get_sign.bc

# ktest-tool --write-ints klee-last/test000001.ktest

# ktest-tool --write-ints klee-last/test000002.ktest

# ktest-tool --write-ints klee-last/test000003.ktest

# export export LD_LIBRARY_PATH=../Coppelia/klee/Release+Asserts/lib/:$LD_LIBRARY_PATH

# gcc -L ../Coppelia/klee/Release+Asserts/lib/ get_sign.c -lkleeRuntest

# KTEST_FILE=klee-last/test000003.ktest ./a.out 

# echo "get_sign(a)" = $?


# Example 2: Regexp.c
# clang -emit-llvm -c -g Regexp.c

# klee --only-output-states-covering-new Regexp.bc


# Example 3: maze.c
#gcc maze.c -o maze

if [ -f maze_klee.bc ]; then
    rm -f maze_klee.bc
fi

#llvm-gcc -c -emit-llvm maze.c -o maze.bc
#lli maze.bc

# Example 4: maze_klee.c
clang -emit-llvm -c -g maze_klee.c 

#klee maze_klee.bc

klee -emit-all-errors --solver-backend=z3 maze_klee.bc

ls -l klee-last/ | grep -A2 -B2 err

