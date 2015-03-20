##############################################################################
# Product:  QP/C, Win32 build, GNU compiler
# Last Updated for Version: 4.5.02
# Date of the Last Update:  Jul 29, 2012
#
#                    Q u a n t u m     L e a P s
#                    ---------------------------
#                    innovating embedded systems
#
# Copyright (C) 2002-2012 Quantum Leaps, LLC. All rights reserved.
#
# This software may be distributed and modified under the terms of the GNU
# General Public License version 2 (GPL) as published by the Free Software
# Foundation and appearing in the file GPL.TXT included in the packaging of
# this file. Please note that GPL Section 2[b] requires that all works based
# on this software must also be made publicly available under the terms of
# the GPL ("Copyleft").
#
# Alternatively, this software may be distributed and modified under the
# terms of Quantum Leaps commercial licenses, which expressly supersede
# the GPL and are specifically designed for licensees interested in
# retaining the proprietary status of their code.
#
# Contact information:
# Quantum Leaps Web site:  http://www.quantum-leaps.com
# e-mail:                  info@quantum-leaps.com
##############################################################################

# examples of invoking this Makefile:
# building configurations: Debug (default), Release, and Spy
# make
# make CONF=rel
# make CONF=spy
#
# cleaning configurations: Debug (default), Release, and Spy
# make clean
# make CONF=rel clean
# make CONF=spy clean
# 
# To control output from compiler/linker, use the following flag 
# If TRACE=0 -->TRACE_FLAG=
# If TRACE=1 -->TRACE_FLAG=@
# If TRACE=something -->TRACE_FLAG=something
TRACEON                     = $(TRACE:0=@)
TRACE_FLAG                  = $(TRACEON:1=)

#-----------------------------------------------------------------------------
# general utilities
#
RM                          = rm -rf
ECHO                        = echo
MKDIR                       = mkdir -p
TARGET_PLATFORM             = POSIX

#-----------------------------------------------------------------------------
# POSIX systems don't need a cross compiler so leave that blank
#
CROSS                       =
CC                          = $(CROSS)gcc
AS                          = $(CROSS)as
OBJCPY                      = $(CROSS)objcopy
RANLIB                      = $(CROSS)ranlib

QPC                         = ../../..
QP_INCDIR                   = $(QPC)/include
QEP_SRCDIR                  = $(QPC)/qep/source
QF_SRCDIR                   = $(QPC)/qf/source
QS_SRCDIR                   = $(QPC)/qs/source
QP_PRTDIR                   = ../

LIBFLAGS                    = rs
CCINC                       = -I$(QP_PRTDIR) -I$(QP_INCDIR) -I$(QF_SRCDIR)

#-----------------------------------------------------------------------------
# build options for various configurations
#

# Common build options for all configs
# -c         : compile only
# -Wall      : all warnings enabled
# -std=gnu99 : enable C99 rules for gnu compilers
# -pthread   : enable thread safe and reentrant calls
# -ffunction-sections/fdata-sections : enable compiler optimizations that allow
#              easy use of -gc-sections when linking to this library                     
CCFLAGS                      = -c -Wall -std=gnu99 -pthread \
                               -ffunction-sections -fdata-sections

ifeq (rel, $(CONF))       # Release configuration ............................

BINDIR                      = rel

# gcc options
# -c    : compile only
# -O    : optimize -c -O2 -Wall -pthread -o$@
#CCFLAGS                     = -c -O2 -Wall -pthread -std=gnu99 -o$@
CCFLAGS                    += -O2

else ifeq (spy, $(CONF))  # Spy configuration ................................

BINDIR                      = spy

# gcc options
# -c    : compile only
# -g    : source level debugging
# -D    : define preprocessor macro  -c -g -O -Wall -pthread -DQ_SPY -o$@
#CCFLAGS                     = -c -g -O -Wall -pthread -DQ_SPY -o$@
CCFLAGS                    += -g -O -DQ_SPY

QS_CSRC                     = qs.c \
	                          qs_.c \
                              qs_blk.c \
                              qs_byte.c \
                              qs_dict.c \
                              qs_f32.c \
                              qs_f64.c \
                              qs_mem.c \
                              qs_str.c \
                              qs_u64.c
                              
QS_OBJS                    = $(patsubst %.c, $(BINDIR)/%.o, $(QS_CSRC))

else                     # default Debug configuration .......................

BINDIR                      = dbg
QP_DEP                      = $(QP_INCDIR)/qassert.h

# gcc options
# -g3   : source level debugging
# -ggdb : include gdb symbols for debugging
CCFLAGS                    += -g3 -ggdb -O
endif

VPATH = $(QEP_SRCDIR) $(QF_SRCDIR) $(QS_SRCDIR) ..
#-----------------------------------------------------------------------------
# QEP src and objects 
#
QEP_CSRC                    = qep.c \
                              qmsm_ini.c \
                              qmsm_dis.c \
                              qmsm_in.c \
                              qfsm_ini.c \
                              qfsm_dis.c \
                              qhsm_ini.c \
                              qhsm_dis.c \
                              qhsm_top.c \
                              qhsm_in.c 
					           
QEP_OBJS                    = $(patsubst %.c, $(BINDIR)/%.o, $(QEP_CSRC))

#-----------------------------------------------------------------------------
# QF src and objects 
#			
QF_CSRC  					= qa_ctor.c \
                              qa_defer.c \
                              qa_fifo.c \
                              qa_lifo.c \
                              qa_get_.c \
                              qa_sub.c \
                              qa_usub.c \
                              qa_usuba.c \
                              qeq_init.c \
                              qeq_fifo.c \
                              qeq_lifo.c \
                              qeq_get.c \
                              qf_act.c \
                              qf_gc.c \
                              qf_log2.c \
                              qf_new.c \
                              qf_pool.c \
                              qf_psini.c \
                              qf_pspub.c \
                              qf_pwr2.c \
                              qf_tick.c \
                              qma_ctor.c \
                              qmp_get.c \
                              qmp_init.c \
                              qmp_put.c \
                              qte_ctor.c \
                              qte_arm.c \
                              qte_darm.c \
                              qte_rarm.c \
                              qte_ctr.c \
                              qf_port.c

QF_OBJS                     = $(patsubst %.c, $(BINDIR)/%.o, $(QF_CSRC))

ALL_OBJS                    = $(QEP_OBJS) $(QF_OBJS) $(QS_OBJS)
#-----------------------------------------------------------------------------
# Build Targets
#
.default: all
all: $(BINDIR) $(BINDIR)/libqp_$(TARGET_PLATFORM)_cs.a

$(BINDIR)/libqp_$(TARGET_PLATFORM)_cs.a: $(BINDIR) $(QEP_OBJS) $(QF_OBJS) $(QS_OBJS)
	@echo --- Creating library $(BINDIR)/libqp_$(TARGET_PLATFORM)_cs.a
	-$(RM) $(BINDIR)/libqp_$(TARGET_PLATFORM)_cs.a
	$(TRACE_FLAG)$(AR) $(LIBFLAGS) $(BINDIR)/libqp_$(TARGET_PLATFORM)_cs.a $(ALL_OBJS)
	$(TRACE_FLAG)$(RANLIB) $(BINDIR)/libqp_$(TARGET_PLATFORM)_cs.a

$(BINDIR)/%.o: %.c
	@echo --- Compiling $(<F)
	$(TRACE_FLAG)$(CC) $(CCFLAGS) $(CCINC) $(INCLUDES) $< -o $@
	
$(BINDIR)/%.o: %.S
	@echo --- Compiling $(<F)
	$(TRACE_FLAG)$(AS) $(ASMFLAGS) $< -o $@

$(BINDIR):
	@echo --- Creating output directory $@
	mkdir $@

# clean ======================================================================
.PHONY : clean
clean:
	@echo --- Cleaning output libs and objects
	-$(RM) $(BINDIR)/*.o
	-$(RM) $(BINDIR)/*.a
	-$(RM) $(BINDIR)
