# OS version
FROM ubuntu:16.04
WORKDIR /home/workspace

ENV DEBIAN_FRONTEND=noninteractive

# Change software source
RUN cp /etc/apt/sources.list /etc/apt/sources.list.bak
RUN echo "deb http://mirrors.tuna.tsinghua.edu.cn/ubuntu/ xenial main restricted" > /etc/apt/sources.list
RUN echo "deb http://mirrors.tuna.tsinghua.edu.cn/ubuntu/ xenial-updates main restricted" >> /etc/apt/sources.list
RUN echo "deb http://mirrors.tuna.tsinghua.edu.cn/ubuntu/ xenial universe" >> /etc/apt/sources.list
RUN echo "deb http://mirrors.tuna.tsinghua.edu.cn/ubuntu/ xenial-updates universe" >> /etc/apt/sources.list
RUN echo "deb http://mirrors.tuna.tsinghua.edu.cn/ubuntu/ xenial multiverse" >> /etc/apt/sources.list
RUN echo "deb http://mirrors.tuna.tsinghua.edu.cn/ubuntu/ xenial-updates multiverse" >> /etc/apt/sources.list
RUN echo "deb http://mirrors.tuna.tsinghua.edu.cn/ubuntu/ xenial-backports main restricted universe multiverse" >> /etc/apt/sources.list
RUN echo "deb http://mirrors.tuna.tsinghua.edu.cn/ubuntu/ xenial-security main restricted" >> /etc/apt/sources.list
RUN echo "deb http://mirrors.tuna.tsinghua.edu.cn/ubuntu/ xenial-security universe" >> /etc/apt/sources.list
RUN echo "deb http://mirrors.tuna.tsinghua.edu.cn/ubuntu/ xenial-security multiverse" >> /etc/apt/sources.list

# Install dependencies
RUN apt-get update
RUN apt-get install -y build-essential curl wget bison flex bc libcap-dev git cmake libboost-all-dev libncurses5-dev libssl-dev python-minimal python-pip python-dev unzip wget zlib1g zlib1g-dev libbz2-dev vim libmpfr-dev libgmp-dev libboost-all-dev autoconf subversion libantlr3c-dev iputils-ping python3-venv kcachegrind
 
RUN export C_INCLUDE_PATH=/usr/include/x86_64-linux-gnu
RUN export CPLUS_INCLUDE_PATH=/usr/include/x86_64-linux-gnu

# Build KLEE with TCMalloc support
RUN apt-get install -y libtcmalloc-minimal4 libgoogle-perftools-dev 

# Install LLVM 3.4
RUN mkdir /home/workspace/llvm-clang
RUN cd /home/workspace/llvm-clang 
RUN wget https://releases.llvm.org/3.4/llvm-3.4.src.tar.gz
RUN tar -xzf llvm-3.4.src.tar.gz 
RUN mv llvm-3.4 llvm
RUN cd llvm/tools
RUN wget https://releases.llvm.org/3.4/clang-3.4.src.tar.gz
RUN tar -xzf clang-3.4.src.tar.gz
RUN mv clang-3.4 clang
RUN cd clang/tools/
RUN wget https://releases.llvm.org/3.4/clang-tools-extra-3.4.src.tar.gz
RUN tar -xzf clang-tools-extra-3.4.src.tar.gz
RUN mv clang-tools-extra-3.4 extra
RUN cd ../../../
RUN cd ./projects/
RUN wget https://releases.llvm.org/3.4/compiler-rt-3.4.src.tar.gz
RUN tar -zxf compiler-rt-3.4.src.tar.gz
RUN mv compiler-rt-3.4 compiler-rt
RUN cd ../../
RUN mkdir llvm-build
RUN cd llvm-build
RUN ../llvm/configure --enable-optimized
RUN make -j$(nproc)
RUN make install
RUN cd ../../

# Install STP solver
RUN git clone https://github.com/stp/minisat.git
RUN cd minisat
RUN mkdir build
RUN cd build
RUN cmake -DSTATICCOMPILE=ON -DCMAKE_INSTALL_PREFIX=/usr/ ../
RUN make install
RUN cd ../../
RUN git clone https://github.com/stp/stp.git
RUN cd stp
RUN git checkout tags/2.1.2
RUN mkdir build
RUN cd build
RUN cmake -DBUILD_SHARED_LIBS:BOOL=OFF -DENABLE_PYTHON_INTERFACE:BOOL=OFF ..
RUN make
RUN make install
RUN cd ../../
RUN ulimit -s unlimited

# Install z3 solver
RUN git clone --branch z3-4.6.0 https://github.com/Z3Prover/z3
RUN cd z3
RUN python scripts/mk_make.py
RUN cd build 
RUN make -j$(nproc)
RUN make install
RUN cd ../../ 

# Install uclibc and POSIX
RUN git clone --branch klee_uclibc_v1.0.0 https://github.com/klee/klee-uclibc
RUN cd klee-uclibc
RUN ./configure --make-llvm-lib
RUN make -j$(nproc)
RUN cd ..

# Build google test sources
RUN curl -OL https://github.com/google/googletest/archive/release-1.7.0.zip  
RUN unzip gtest-1.7.0.zip  
RUN cd googletest-release-1.7.0  
RUN cmake .  
RUN make -j$(nproc) 
RUN cd ..

# Coppelia
RUN git clone https://github.com/rzhang2285/Coppelia.git
RUN cd Coppelia
RUN git submudole update --init --recursive

# Install klee which is modified for Coppelia
RUN cd klee
RUN ./configure --with-z3=/home/workspace/z3/build --with-stp=/home/workspace/stp/build --with-uclibc=/home/workspace/klee-uclibc --enable-posix-runtime --prefix=/home/workspace/Coppelia/klee/build/
RUN make -j$(nproc)
RUN make install 
RUN cd ../../

# Install original klee v1.4.0
RUN git clone https://github.com/klee/klee.git
RUN cd klee
RUN git checkout tags/v1.4.0
RUN ./configure --with-z3=/home/workspace/z3/build --with-stp=/home/workspace/stp/build --with-uclibc=/home/workspace/klee-uclibc --enable-posix-runtime
RUN make -j$(nproc)
RUN make install 
RUN cd ../ 

# Intall Verilator
RUN git clone http://git.veripool.org/git/verilator
RUN cd verilator
RUN git checkout v3.900
RUN autoconf
RUN ./configure
RUN make
RUN make install
RUN cd ..

# Upate the Makefile with your own KLEE_ROOT and KLEE_INCLUDE
RUN cd Coppelia
RUN sed -i "s|KLEE_ROOT = .*|KLEE_ROOT = /home/workspace/Coppelia/klee|" verilator/verilator.mk
RUN sed -i "s|KLEE_INCLUDE = .*|KLEE_INCLUDE = /home/workspace/Coppelia/klee/include|" verilator/verilator.mk

# Install WLLVM to extract bytecode for KLEE symbolic execution
RUN git clone https://github.com/travitch/whole-program-llvm
RUN cd whole-program-llvm
RUN git checkout 26b52aabc0fc55ae4a7eb5986b1d433c544ab4da
RUN pip install -e .
RUN export LLVM_COMPILER=clang
RUN export WLLVM_OUTPUT=WARNING

# Update Verilator's Makefile with klee and WLLVM 
RUN mv /usr/local/share/verilator/include/verilated.mk /usr/local/share/verilator/include/verilated.mk.bak
RUN cp verilator/verilated.mk /usr/local/share/verilator/include
RUN cd ../