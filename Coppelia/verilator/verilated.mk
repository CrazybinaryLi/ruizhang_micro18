# -*- Makefile -*-
######################################################################
# DESCRIPTION: Makefile commands for all verilated target files
#
# Copyright 2003-2016 by Wilson Snyder. Verilator is free software; you can
# redistribute it and/or modify it under the terms of either the GNU Lesser
# General Public License Version 3 or the Perl Artistic License Version 2.0.
######################################################################

PERL = /usr/bin/perl
#CXX = g++
#LINK = g++
#CXX = /playpen/rzhang/Klee/klee/scripts/klee-clang
#LINK = /playpen/rzhang/Klee/klee/scripts/klee-clang
CXX = wllvm++
LINK = wllvm++
AR     = ar
RANLIB = ranlib

CFG_WITH_CCWARN = no
CFG_WITH_LONGTESTS = no

KLEE_ROOT = /home/workspace/Coppelia/klee
KLEE_INCLUDE = /home/workspace/Coppelia/klee/include

# Compiler flags to use to turn off unused and generated code warnings, such as -Wno-div-by-zero
CFG_CXXFLAGS_NO_UNUSED =  -Wno-char-subscripts -Wno-parentheses-equality -Wno-sign-compare -Wno-uninitialized -Wno-unused-but-set-variable -Wno-unused-parameter -Wno-unused-variable

######################################################################
# Programs

SP_PREPROC	= sp_preproc
SP_INCLUDER	= $(VERILATOR_INCLUDER)
VERILATOR_COVERAGE = $(PERL) $(VERILATOR_ROOT)/bin/verilator_coverage
VERILATOR_INCLUDER = $(PERL) $(VERILATOR_ROOT)/bin/verilator_includer

######################################################################
# Make checks

ifneq ($(words $(CURDIR)),1)
 $(error Unsupported: GNU Make cannot build in directories containing spaces, build elsewhere: '$(CURDIR)')
endif

######################################################################
# C Preprocessor flags

# Add -MMD -MP if you're using a recent version of GCC.
VK_CPPFLAGS_ALWAYS += \
		-MMD \
		-I$(VERILATOR_ROOT)/include \
		-I$(VERILATOR_ROOT)/include/vltstd \
		-I$(KLEE_ROOT)/include \
		-DVL_PRINTF=printf \
		-DVM_COVERAGE=$(VM_COVERAGE) \
		-DVM_SC=$(VM_SC) \
		-DVM_TRACE=$(VM_TRACE) \
#		$(CFG_CXXFLAGS_NO_UNUSED) \

ifeq ($(CFG_WITH_CCWARN),yes)	# Local... Else don't burden users
VK_CPPFLAGS_WALL += -Wall \
		-Werror
endif

CPPFLAGS += -I. $(VK_CPPFLAGS_WALL) $(VK_CPPFLAGS_ALWAYS)

VPATH += ..
VPATH += $(VERILATOR_ROOT)/include
VPATH += $(VERILATOR_ROOT)/include/vltstd

#OPT = -ggdb -DPRINTINITSTR -DDETECTCHANGE
#OPT = -ggdb -DPRINTINITSTR
CPPFLAGS += $(OPT)

CPPFLAGS += $(M32)
LDFLAGS  += $(M32)

# Allow upper level user makefiles to specify flags they want.
# These aren't ever set by Verilator, so users are free to override them.
CPPFLAGS += $(USER_CPPFLAGS)
LDFLAGS  += $(USER_LDFLAGS)
LDLIBS   += $(USER_LDLIBS)

# Add flags from -CFLAGS and -LDFLAGS on Verilator command line
CPPFLAGS += $(VM_USER_CFLAGS)
LDFLAGS  += $(VM_USER_LDFLAGS)
LDLIBS   += $(VM_USER_LDLIBS)

# For klee
#CPPFLAGS += -Wl,-rpath=$(KLEE_ROOT)/Release+Asserts/lib -lkleeRunTest
#CPPFLAGS += -lkleeRunTest

# See the benchmarking section of bin/verilator.
# Support class optimizations.  This includes the tracing and symbol table.
# SystemC takes minutes to optimize, thus it is off by default.
#OPT_SLOW =
# Fast path optimizations.  Most time is spent in these classes.
#OPT_FAST = -O2 -fstrict-aliasing
#OPT_FAST = -O
#OPT_FAST =

#######################################################################
##### Aggregates

VM_CLASSES += $(VM_CLASSES_FAST) $(VM_CLASSES_SLOW)
VM_SUPPORT += $(VM_SUPPORT_FAST) $(VM_SUPPORT_SLOW)

#######################################################################
##### SystemC or SystemPerl builds

ifeq ($(VM_SP_OR_SC),1)
  CPPFLAGS += $(SYSTEMC_CXX_FLAGS) -I$(SYSTEMC_INCLUDE)
  LDFLAGS  += $(SYSTEMC_CXX_FLAGS) -L$(SYSTEMC_LIBDIR)
  SC_LIBS   = -lsystemc
 ifneq ($(wildcard $(SYSTEMC_LIBDIR)/*numeric_bit*),)
  # Systemc 1.2.1beta
  SC_LIBS   += -lnumeric_bit -lqt
 endif
endif

#######################################################################
##### SystemPerl builds

ifeq ($(VM_SP),1)
  CPPFLAGS += -I$(SYSTEMPERL_INCLUDE) -DSYSTEMPERL
  VPATH    +=   $(SYSTEMPERL_INCLUDE)
  LIBS   += -lm -lstdc++

  VK_CLASSES_SP = $(addsuffix .sp, $(VM_CLASSES))

  # This rule is called manually by the upper level makefile
  preproc:
	 @echo "      SP Preprocess" $(basename $(VM_CLASSES)) ...
	 $(SP_PREPROC) -M sp_preproc.d --tree $(VM_PREFIX).sp_tree \
		--preproc $(VK_CLASSES_SP)
else
  preproc:
endif

#######################################################################
##### C/H builds

LIBS   += -lm -lstdc++ 
LIBS   += -L$(KLEE_ROOT)/Release+Asserts/lib -lkleeRuntest 

#######################################################################
# Overall Objects Linking

VK_CLASSES_H   = $(addsuffix .h, $(VM_CLASSES))
VK_CLASSES_CPP = $(addsuffix .cpp, $(VM_CLASSES))

VK_SUPPORT_CPP = $(addsuffix .cpp, $(VM_SUPPORT))

VK_USER_OBJS   = $(addsuffix .o, $(VM_USER_CLASSES))

VK_GLOBAL_OBJS = $(addsuffix .o, $(VM_GLOBAL_FAST) $(VM_GLOBAL_SLOW))

ifneq ($(VM_PARALLEL_BUILDS),1)
  # Fast building, all .cpp's in one fell swoop
  # This saves about 5 sec per module, but can be slower if only a little changes
  VK_OBJS += $(VM_PREFIX)__ALLcls.o   $(VM_PREFIX)__ALLsup.o
  all_cpp:   $(VM_PREFIX)__ALLcls.cpp $(VM_PREFIX)__ALLsup.cpp
  $(VM_PREFIX)__ALLcls.cpp: $(VK_CLASSES_CPP)
	$(VERILATOR_INCLUDER) -DVL_INCLUDE_OPT=include $^ > $@
  $(VM_PREFIX)__ALLsup.cpp: $(VK_SUPPORT_CPP)
	$(VERILATOR_INCLUDER) -DVL_INCLUDE_OPT=include $^ > $@
else
  #Slow way of building... Each .cpp file by itself
  VK_OBJS += $(addsuffix .o, $(VM_CLASSES) $(VM_SUPPORT))
endif

$(VM_PREFIX)__ALL.a: $(VK_OBJS)
	@echo "      Archiving" $@ ...
	$(AR) r $@ $^
	$(RANLIB) $@

######################################################################
### Compile rules

ifneq ($(VM_DEFAULT_RULES),0)
$(VM_PREFIX)__ALLsup.o: $(VM_PREFIX)__ALLsup.cpp
	$(OBJCACHE) $(CXX) $(CXXFLAGS) $(CPPFLAGS) $(OPT_SLOW) -c -o $@ $<

$(VM_PREFIX)__ALLcls.o: $(VM_PREFIX)__ALLcls.cpp
	$(OBJCACHE) $(CXX) $(CXXFLAGS) $(CPPFLAGS) $(OPT_FAST) -c -o $@ $<

$(VM_PREFIX)%__Slow.o: $(VM_PREFIX)%__Slow.cpp
	$(OBJCACHE) $(CXX) $(CXXFLAGS) $(CPPFLAGS) $(OPT_SLOW) -c -o $@ $<

$(VM_PREFIX)%.o: $(VM_PREFIX)%.cpp
	$(OBJCACHE) $(CXX) $(CXXFLAGS) $(CPPFLAGS) $(OPT_FAST) -c -o $@ $<
endif

#Default rule embedded in make:
#.cpp.o:
#	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -c -o $@ $<

######################################################################
### Debugging

debug-make::
	@echo
	@echo VM_PREFIX:  $(VM_PREFIX)
	@echo VM_CLASSES_FAST: $(VM_CLASSES_FAST)
	@echo VM_CLASSES_SLOW: $(VM_CLASSES_SLOW)
	@echo VM_SUPPORT_FAST: $(VM_SUPPORT_FAST)
	@echo VM_SUPPORT_SLOW: $(VM_SUPPORT_SLOW)
	@echo VM_GLOBAL_FAST: $(VM_GLOBAL_FAST)
	@echo VM_GLOBAL_SLOW: $(VM_GLOBAL_SLOW)
	@echo CPPFLAGS: $(CPPFLAGS)
	@echo

######################################################################
### Detect out of date files and rebuild.

DEPS := $(wildcard *.d)
ifneq ($(DEPS),)
include $(DEPS)
endif
