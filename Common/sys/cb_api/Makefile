##############################################################################
# Product: Makefile for Project
# 
#                             Datacard 
#                    ---------------------------
#
# Copyright (C) 2015 Datacard. All rights reserved.
#
##############################################################################
# examples of invoking this Makefile:
# building configurations: Debug (default), Release, and Spy
# make MCU=cortex-m4-fpv4-sp-d16
# make MCU=cortex-m4-fpv4-sp-d16 CONF=rel
# make MCU=cortex-m4-fpv4-sp-d16 CONF=spy
#
# cleaning configurations: Debug (default), Release, and Spy
# make MCU=cortex-m4-fpv4-sp-d16 clean
# make MCU=cortex-m4-fpv4-sp-d16 CONF=rel clean
# make MCU=cortex-m4-fpv4-sp-d16 CONF=spy clean
# 
# To control output from compiler/linker, use the following flag 
# If TRACE=0 -->TRACE_FLAG=
# If TRACE=1 -->TRACE_FLAG=@
# If TRACE=something -->TRACE_FLAG=something
TRACEON                 = $(TRACE:0=@)
TRACE_FLAG              = $(TRACEON:1=)

# Output file basename (should be the same as the library name)
PROJECT_NAME            = libCBCommApi

# Set up variable for appending later
DEFINES                 = 

#------------------------------------------------------------------------------
#  CHECK TARGET
#------------------------------------------------------------------------------
ifeq (ppc, $(TARGET)) 

#------------------------------------------------------------------------------
#  C/C++ TOOLCHAIN SETUP FOR POWER PC
#------------------------------------------------------------------------------
CROSS                   = $(PPC_TOOL)

# Build options
MFLAGS                  = -mcpu=e300c3 -fomit-frame-pointer -pipe
AFLAGS                  = -mcpu=e300c3
LIBDIR                  = ppc_lib

else ifeq (arm, $(TARGET))

#------------------------------------------------------------------------------
#  C/C++ TOOLCHAIN SETUP FOR ARM CORTEX Mx
#------------------------------------------------------------------------------

#------------------------------------------------------------------------------
#  MCU SETUP - This specifies which core to pass down to all the other makefiles
#  and to compile options for different ARM cortex-m processors.
#------------------------------------------------------------------------------

# defaults
ARM_CORE                = cortex-m3
MFLAGS                  = -mcpu=$(ARM_CORE) -mthumb -mfloat-abi=soft
AFLAGS                  = -mcpu=$(ARM_CORE)

ifeq (cortex-m0, $(MCU))
    ARM_CORE            = cortex-m0
    MFLAGS              = -mcpu=$(ARM_CORE) -mthumb -mfloat-abi=soft
    AFLAGS              = -mcpu=$(ARM_CORE)
else ifeq (cortex-m0plus, $(MCU))
    ARM_CORE            = cortex-m0plus
    MFLAGS              = -mcpu=$(ARM_CORE) -mthumb -mfloat-abi=soft
    AFLAGS              = -mcpu=$(ARM_CORE)
else ifeq (cortex-m3, $(MCU))
    ARM_CORE            = cortex-m3
    MFLAGS              = -mcpu=$(ARM_CORE) -mthumb -mfloat-abi=soft
    AFLAGS              = -mcpu=$(ARM_CORE)
else ifeq (cortex-m4-vfp, $(MCU))
    ARM_CORE            = cortex-m4
    ARM_FPU             = fpv4-sp-d16
    MFLAGS              = -mcpu=$(ARM_CORE) -mfpu=$(ARM_FPU) -mfloat-abi=softfp \
						  -mlittle-endian -mthumb
    AFLAGS              = -mcpu=$(ARM_CORE) -defsym=FPU_VFP_V4_SP_D16=1
    DEFINES            += -D__VFP_FP__
else ifeq (cortex-m4-fpv4-sp-d16, $(MCU))
    ARM_CORE            = cortex-m4
    ARM_FPU             = fpv4-sp-d16
    MFLAGS              = -mcpu=$(ARM_CORE) -mfpu=$(ARM_FPU) -mfloat-abi=hard \
	                      -mlittle-endian -mthumb
    AFLAGS              = -mcpu=$(ARM_CORE) -defsym=FPU_VFP_V4_SP_D16=1
    DEFINES            += -D__VFP_FP__	
else
    $(error Must specify the MCU, like MCU=cortex-m0 )
endif

CROSS                   = arm-none-eabi-
LIBDIR                  = $(TARGET)_lib

else ifeq (win32, $(TARGET)) 
#------------------------------------------------------------------------------
#  C/C++ TOOLCHAIN SETUP FOR WIN32
#------------------------------------------------------------------------------
# This will detect whether the linux system is 64 or 32 bit since the mingw 
# compiler's name is dependent on it
LBITS                  := $(shell getconf LONG_BIT)
ifeq ($(LBITS),64)
   CROSS                = x86_64-w64-mingw32-
else
   CROSS                = i686-w64-mingw32-
endif

# Build options
ARM_CORE                = $(TARGET)
MFLAGS                  = 
LIBDIR                  = $(TARGET)_lib

else #ia32 or none specified
#------------------------------------------------------------------------------
#  C/C++ TOOLCHAIN SETUP ia32 and ia64 (default)
#------------------------------------------------------------------------------
CROSS                   = 
TARGET                  = posix
# Build options
ARM_CORE                = $(TARGET)
MFLAGS                  =
LIBDIR                  = $(TARGET)_lib
LIBFLAGS                = rcs

CFLAGS                  = -pthread -fPIC
CPPFLAGS                = -pthread -fPIC

endif

#-------------------------------------------------------------------------------
# Now that CROSS has been defined, set up the toolchain
#-------------------------------------------------------------------------------
CC                      = $(CROSS)gcc
CPP                     = $(CROSS)g++
AS                      = $(CROSS)as
LINK                    = $(CROSS)gcc
OBJCPY                  = $(CROSS)objcopy
SIZE                    = $(CROSS)size
RANLIB                  = $(CROSS)ranlib
AR                      = $(CROSS)ar

RM                      = rm -rf
ECHO                    = echo
MKDIR                   = mkdir -p

#-------------------------------------------------------------------------------
# build options for various configurations
#-------------------------------------------------------------------------------

# Common options for all configurations. 
LIBFLAGS                = rcs

# Specific options depending on the build configuration
ifeq (rel, $(CONF))       # Release configuration ............................

BIN_DIR 	:= rel
DEFINES		+= -DNDEBUG
ASFLAGS      = $(MFLAGS)
CFLAGS      += $(MFLAGS) -std=gnu99 -Wall -ffunction-sections -fdata-sections \
			   -Os $(INCLUDES) $(DEFINES)

CPPFLAGS    += $(MFLAGS) \
			   -Wall -fno-rtti -fno-exceptions \
			   -Os $(INCLUDES) $(DEFINES)

else ifeq (spy, $(CONF))  # Spy configuration ................................

BIN_DIR 	:= spy
DEFINES		+= -DQ_SPY
ASFLAGS 	 = $(MFLAGS)
CFLAGS 		+= $(MFLAGS) -std=gnu99 -Wall -ffunction-sections -fdata-sections \
			   -Os -O $(INCLUDES) $(DEFINES)

CPPFLAGS 	+= $(MFLAGS) \
			   -Wall -fno-rtti -fno-exceptions \
			   -O $(INCLUDES) $(DEFINES)

else                     # default Debug configuration .......................

BIN_DIR 	:= dbg
ASFLAGS      = -g3 -ggdb $(MFLAGS)
CFLAGS 		+= $(MFLAGS) -std=gnu99 -Wall -ffunction-sections -fdata-sections \
               -Wl,--gc-sections \
			   -g -ggdb -O0 $(INCLUDES) $(DEFINES)

CPPFLAGS 	+= $(MFLAGS) \
			   -Wall -rtti -fno-exceptions \
			   -g3 -ggdb -O0 $(INCLUDES) $(DEFINES)

endif


#------------------------------------------------------------------------------
#  Which proto files to use.
#------------------------------------------------------------------------------
PROTO_FILES     = CBMsgs.proto 

#------------------------------------------------------------------------------
# Files
#------------------------------------------------------------------------------
SRC_FILES       = dfuse.c \
                  util.c

# C source files (will be generated or already there)
C_SRCS          = $(patsubst %.proto, %.c, $(PROTO_FILES)) $(SRC_FILES)
C_CORE_SRCS     = protobuf_core.c

#------------------------------------------------------------------------------
#  Common build options
#------------------------------------------------------------------------------
C_OBJS          = $(patsubst %.c, %.o, $(C_SRCS))
C_OBJS_EXT      = $(addprefix $(BIN_DIR)/, $(C_OBJS))
C_DEPS_EXT      = $(patsubst %.o, %.d, $(C_OBJS_EXT))

C_CORE_OBJS     = $(patsubst %.c, %.o, $(C_CORE_SRCS))
C_CORE_OBJS_EXT = $(addprefix $(BIN_DIR)/, $(C_CORE_OBJS))
C_CORE_DEPS_EXT = $(patsubst %.o, %.d, $(C_CORE_OBJS_EXT))

#------------------------------------------------------------------------------
# Directories
#------------------------------------------------------------------------------
PROTO_FILES_DIR	= proto
GEN_SOURCES_DIR	= gen_src
CMPLED_OBJS_DIR	= obj_$(TARGET)

SOURCES_DIR		= src

# source directories
VPATH	 	 	= $(GEN_SOURCES_DIR) $(SOURCES_DIR)

# include directories
INCLUDES     	= -I$(GEN_SOURCES_DIR) -I$(SOURCES_DIR)

# Where the objects will go
BIN_DIR 		= $(CMPLED_OBJS_DIR)

LIB_NAME        = $(PROJECT_NAME)_$(TARGET)
LIB_CORE        = libprotobuf_core_$(TARGET)

#------------------------------------------------------------------------------
#  JAVA SETUP
#------------------------------------------------------------------------------
JAVA            = java
JFLAGS          = -jar
JPROTO_FLAGS    = --c_out=$(GEN_SOURCES_DIR)
PROTOBUF_DIR    = protobuf_shared
JAR_FILE        = $(PROTOBUF_DIR)/protoc-1.0M2.jar

#------------------------------------------------------------------------------
#  PYTHON SETUP
#------------------------------------------------------------------------------
PYTHON          = python
PFLAGS          = -i $(GEN_SOURCES_DIR)/CBMsgs.c
LIB_SPLITTER    = lib_splitter_shared/src/protobuf_lib_splitter.py

#-------------------------------------------------------------------------------
# Build rules
#-------------------------------------------------------------------------------
# .PHONY: all cleanall clean cleanlibs cleandirs regenerate

# This is needed to prevent make from deleting the generated *.c files
# Uncomment this if you want to keep the generated *.c files.
#.SECONDARY: $(GEN_SOURCES_DIR)/rfid_board_api.c

# Make sure a target is specified
ifeq ($(MAKECMDGOALS), )
$(error No make target specified.  Make should not be called without targets)
endif

default: all
# Make sure not to generate dependencies when doing cleans
NODEPS:=clean cleanall cleanlibs cleandirs regenerate $(BIN_DIR) $(GEN_SOURCES_DIR) $(LIBDIR)
ifeq (0, $(words $(findstring $(MAKECMDGOALS), $(NODEPS))))
-include $(C_DEPS_EXT) $(C_CORE_DEPS_EXT)
endif

#-------------------------------------------------------------------------------
# Make Targets
#-------------------------------------------------------------------------------
all: $(BIN_DIR) $(BIN_DIR)/CBMsgs.o $(BIN_DIR)/protobuf_core.o $(LIBDIR)/$(LIB_NAME).a $(LIBDIR)/$(LIB_CORE).a
	@echo --- Creating library $(LIBDIR)/$(LIB_CORE).a
	$(TRACE_FLAG)$(RANLIB) $(LIBDIR)/$(LIB_CORE).a
	$(TRACE_FLAG)$(RANLIB) $(LIBDIR)/$(LIB_NAME).a

$(GEN_SOURCES_DIR)/CBMsgs.c: $(PROTO_FILES_DIR)/CBMsgs.proto 
	@echo --- Regenerating code from proto files
	$(TRACE_FLAG)$(MKDIR) $(GEN_SOURCES_DIR)
	$(TRACE_FLAG)$(JAVA) $(JFLAGS) $(JAR_FILE) $(JPROTO_FLAGS) -I=$(PROTO_FILES_DIR) $(PROTO_FILES)
	@echo --- Splitting out core functionality
	$(TRACE_FLAG)$(PYTHON) $(LIB_SPLITTER) $(PFLAGS)
	
$(LIBDIR)/$(LIB_NAME).a:  $(C_OBJS_EXT) $(LIBDIR)
	@echo --- Compiling Protocol library $(<F)
	$(TRACE_FLAG)$(AR) $(LIBFLAGS) $(LIBDIR)/$(LIB_NAME).a $(C_OBJS_EXT)

$(LIBDIR)/$(LIB_CORE).a:  $(C_CORE_OBJS_EXT) $(LIBDIR)
	@echo --- Compiling Core library $(<F)
	$(TRACE_FLAG)$(AR) $(LIBFLAGS) $(LIBDIR)/$(LIB_CORE).a $(C_CORE_OBJS_EXT)
	
$(BIN_DIR)/CBMsgs.o : $(GEN_SOURCES_DIR)/CBMsgs.c 
	@echo --- Compiling $(<F)
	$(TRACE_FLAG)$(CC) $(CFLAGS) -c $< -o $@
	
$(BIN_DIR)/protobuf_core.o : $(GEN_SOURCES_DIR)/protobuf_core.c 
	@echo --- Compiling $(<F)
	$(TRACE_FLAG)$(CC) $(CFLAGS) -c $< -o $@
	
$(BIN_DIR)/%.o : $(SOURCES_DIR)/%.c 
	@echo --- Compiling $(<F)
	$(TRACE_FLAG)$(CC) $(CFLAGS) -c $< -o $@

$(BIN_DIR) $(GEN_SOURCES_DIR) $(LIBDIR):
	@echo --- Creating directory $@
	$(TRACE_FLAG)$(MKDIR) $@

#-------------------------------------------------------------------------------
# Clean Targets
#-------------------------------------------------------------------------------
clean:
	@echo --- Cleaning obj and dep files ONLY
	$(TRACE_FLAG)-$(RM) *.o *.d $(BIN_DIR)/*.o $(BIN_DIR)/*.d $(CMPLED_OBJS_DIR)
    
cleanlibs: clean
	@echo --- Cleaning ALL generated libraries
	$(TRACE_FLAG)$(RM) $(BIN_DIR)/*.a $(CMPLED_OBJS_DIR)

cleandirs: cleanlibs
	@echo --- Cleaning generated directories
	$(TRACE_FLAG)$(RM) $(GEN_SOURCES_DIR) ppc_lib arm_lib win32_lib posix_lib $(BIN_DIR) $(CMPLED_OBJS_DIR)
	
cleanall: cleandirs
	@echo --- Cleaning generated *.h and *.c files
	$(TRACE_FLAG)$(RM) *.c *.h  $(GEN_SOURCES_DIR)/*.c  $(GEN_SOURCES_DIR)/*.h ppc_lib arm_lib win32_lib posix_lib $(CMPLED_OBJS_DIR)
	
show:
	@echo --- Printing Variables ---
	@echo PROTO_FILES = $(PROTO_FILES)
	@echo C_SRCS = $(C_SRCS)
	@echo C_OBJS_EXT = $(C_OBJS_EXT)
	@echo C_DEPS_EXT = $(C_DEPS_EXT)
	@echo LIBS = $(LIBS)
	@echo CFLAGS = $(CFLAGS)
