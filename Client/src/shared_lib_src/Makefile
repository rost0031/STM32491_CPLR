##############################################################################
# Product: Makefile for DC3 Coupler Board Client shared dll/so library. 
#
#                             Datacard 
#                    ---------------------------
#
# Copyright (C) 2015 Datacard. All rights reserved.
#
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

# To compile for windows, use TARGET=win32
# Default compiles for Linux/Cygwin
# 
# To control output from compiler/linker, use the following flag 
# If TRACE=0 -->TRACE_FLAG=
# If TRACE=1 -->TRACE_FLAG=@
# If TRACE=something -->TRACE_FLAG=something
TRACE                       = 1
TRACEON                     = $(TRACE:0=@)
TRACE_FLAG                  = $(TRACEON:1=)

# Output file basename (should be the same as the directory name)
PROJECT_DLL                 = DC3Client
BOOST_LIB_VER               = 1.57.0

#------------------------------------------------------------------------------
#  TOOLCHAIN SETUP - Depends on TARGET
#------------------------------------------------------------------------------
ifeq (win32, $(TARGET)) # Cross compile for windows from Ubuntu-12.04

   # This will detect whether the linux system is 64 or 32 bit since the mingw 
    # compiler's name is dependent on it
    LBITS                   = $(shell getconf LONG_BIT)
    ifeq ($(LBITS),64)
        CROSS_PATH          = x86_64-w64-mingw32
        CROSS               = $(CROSS_PATH)-
    else
        CROSS_PATH          = i686-w64-mingw32
        CROSS               = $(CROSS_PATH)-
    endif
 
    QPLIBMOD                = WIN32
#    BOOSTLIBMOD             = -mt-s
#    BOOSTLIBMOD             = -mt.dll
#    BOOST_TH_LIBMOD         = _$(TARGET)

else                    # Compile natively on Linux and/or Cygwin
    CROSS                   = 
    QPLIBMOD                = posix
    BOOSTLIBMOD             = -mt-s
    BOOST_TH_LIBMOD         = 
    TARGET                  = posix
endif

#------------------------------------------------------------------------------
#  TOOLCHAIN SETUP - Common for all targets
#------------------------------------------------------------------------------
CC                          = $(CROSS)g++
CPP                         = $(CROSS)g++
AS                          = $(CROSS)as
LINK                        = $(CROSS)g++
OBJCPY                      = $(CROSS)objcopy
RANLIB					    = $(CROSS)ranlib
RM                          = rm -rf
ECHO                        = echo
MKDIR                       = mkdir

#-----------------------------------------------------------------------------
# DIRECTORIES
#-----------------------------------------------------------------------------
# Top level directories
SRC_DIR                     = .
COMMON_CLI_SRC_DIR          = ../../../Common

# External library includes
EXT_LIB_DIR                 = ../../lib
EXT_LIB_BIN_DIR             = $(EXT_LIB_DIR)/$(TARGET)
EXT_LIB_INC_DIR             = $(EXT_LIB_DIR)/inc

# app, bsp, and sys directories common between firmware and client
COMMON_CLI_BSP_DIR          = $(COMMON_CLI_SRC_DIR)/bsp
COMMON_CLI_SYS_DIR          = $(COMMON_CLI_SRC_DIR)/sys

# app, bsp, sys, dll, and api directories 
API_DIR                     = $(SRC_DIR)/api
BSP_DIR                     = $(SRC_DIR)/bsp
SYS_DIR                     = $(SRC_DIR)/sys

# Coupler Board API directory
DC3_API_DIR                  = $(COMMON_CLI_SYS_DIR)/DC3_api
DC3_API_GEN_SRC_DIR          = $(DC3_API_DIR)/gen_src
DC3_API_SRC_DIR              = $(DC3_API_DIR)/src
DC3_API_INC_DIR              = $(DC3_API_DIR)/inc
DC3_API_LIB_DIR              = $(DC3_API_DIR)/$(TARGET)_lib

# Boost library
BOOST_INC_DIR               = $(EXT_LIB_INC_DIR)
BOOST_LIB_DIR               = $(EXT_LIB_BIN_DIR)

# Base64 encoding module
BASE64_DIR                  = $(COMMON_CLI_SYS_DIR)/libb64

#-----------------------------------------------------------------------------
# SOURCE VIRTUAL DIRECTORIES
#-----------------------------------------------------------------------------
VPATH                       = $(API_DIR) \
                              $(BSP_DIR) \
                              $(SYS_DIR) \
                              $(BASE64_DIR)

#-----------------------------------------------------------------------------
# INCLUDE DIRECTORIES
#-----------------------------------------------------------------------------
INCLUDES                    = -I$(SRC_DIR) \
                              -I$(API_DIR) \
                              -I$(BSP_DIR) \
                              -I$(SYS_DIR) \
                              \
                              -I$(DC3_API_SRC_DIR) \
                              -I$(DC3_API_INC_DIR) \
                              -I$(DC3_API_GEN_SRC_DIR) \
                              \
                              -I$(BASE64_DIR) \
                              \
                              -I$(BOOST_INC_DIR) \
                              \
                              -I$(COMMON_CLI_SYS_DIR)

#-----------------------------------------------------------------------------
# DEFINES
#-----------------------------------------------------------------------------
DEFINES                     = -DDFUSE -DBUILDING_CLIENT_DLL

#-----------------------------------------------------------------------------
# FILES
#-----------------------------------------------------------------------------

# C source files
C_SRCS                      = cdecode.c \
                              cencode.c \
                              base64_wrapper.c

# C++ source files
CPP_SRCS                    = serial.cpp \
                              udp.cpp \
                              comm.cpp \
                              fwLdr.cpp \
                              ClientApi.cpp \
                              LogStub.cpp \
                              msg_utils.cpp

#-----------------------------------------------------------------------------
# BUILD OPTIONS FOR VARIOUS CONFIGURATIONS
#-----------------------------------------------------------------------------
ifeq (rel, $(CONF))       # Release configuration ............................

    BIN_DIR                := rel_$(TARGET)
    DEFINES                += -DNDEBUG
    ASFLAGS                 = 
    CFLAGS                  = -c -std=gnu99 -Wall $(INCLUDES) $(DEFINES)

    CPPFLAGS                = -O2 -Wall -std=gnu++11 -c -Wall \
                              -Wno-write-strings $(INCLUDES) $(DEFINES)

else ifeq (spy, $(CONF))  # Spy configuration ................................

    BIN_DIR                := spy_$(TARGET)
    DEFINES                += -DQ_SPY
    ASFLAGS                 = 
    CFLAGS                  = -c -std=gnu99 -Wall \
                              -g $(INCLUDES) $(DEFINES)

    CPPFLAGS                = -O0 -g3 -Wall -std=gnu++11 -c -Wall \
                              -Wno-write-strings -g $(INCLUDES) $(DEFINES)

else                     # default Debug configuration .......................

	CONF                    = dbg
    BIN_DIR                := dbg_$(TARGET)
    ASFLAGS                 = 
    CFLAGS                  = -c -g3 -ggdb -std=gnu99 -Wall \
                               $(INCLUDES) $(DEFINES)

    CPPFLAGS                = -O0 -g3 -ggdb  -std=gnu++11 -c -Wall \
                              -Wno-write-strings $(INCLUDES) $(DEFINES)
endif

# Both builds need these flags for making a shared library
CFLAGS                     += -shared
CPPFLAGS                   += -shared

API_STATIC_LIBS             = $(DC3_API_LIB_DIR)/libDC3CommApi_$(TARGET).a \
                              $(DC3_API_LIB_DIR)/libprotobuf_core_$(TARGET).a \
                              $(BOOST_LIB_DIR)/libboost_thread$(BOOST_TH_LIBMOD)$(BOOSTLIBMOD).a \
                              $(BOOST_LIB_DIR)/libboost_system$(BOOSTLIBMOD).a
                              
LIB_PREFIX                  = lib
                              
# Modify the INCLUDES to point to the boost libraries compiled for windows
ifeq (win32, $(TARGET))
    LINKFLAGS               = -static-libgcc -static-libstdc++ \
                              -Wl,--out-implib=$(TARGET_DLL).a \
                              -Wl,--export-all-symbols \
                              -Wl,--enable-auto-import \
                              -Wl,--whole-archive $(API_STATIC_LIBS) \
                              -Wl,--no-whole-archive
  
    LIBS                   += -lws2_32 # Needed for win32 builds only
    DEFINES                += -DBOOST_THREAD_USE_LIB -DWIN32COMPILE -DBOOST_ALL_NO_LIB -D_SECURE_SCL=0
    DLL_EXT                 = dll
else
    LINKFLAGS               = -Wl,-soname,$(LIB_PREFIX)$(PROJECT_DLL).$(DLL_EXT) \
                              -Wl,--whole-archive $(API_STATIC_LIBS) \
                              -Wl,--no-whole-archive
                            
    LIBS                   += -lpthread  # This is needed on newer Ubuntu systems but doesn't hurt older ones or Cygwin
    DEFINES                += 
    DLL_EXT                 = so
    LIB_PREFIX              = lib
  
    # Linux shared libs need these flags 
    CFLAGS                 += -pthread -fPIC
    CPPFLAGS               += -pthread -fPIC
endif

TARGET_DLL                  = $(BIN_DIR)/$(LIB_PREFIX)$(PROJECT_DLL).$(DLL_EXT)

C_OBJS                      = $(patsubst %.c,%.o,$(C_SRCS))
C_OBJS_EXT                  = $(addprefix $(BIN_DIR)/, $(C_OBJS))
C_DEPS_EXT                  = $(patsubst %.o, %.d, $(C_OBJS_EXT))

CPP_OBJS                    = $(patsubst %.cpp,%.o,$(CPP_SRCS))
CPP_OBJS_EXT                = $(addprefix $(BIN_DIR)/, $(CPP_OBJS))
CPP_DEPS_EXT                = $(patsubst %.o, %.d, $(CPP_OBJS_EXT))

#-----------------------------------------------------------------------------
# BUILD TARGETS
#-----------------------------------------------------------------------------

.PHONY: build_libs build_dll all clean clean_with_libs build_DC3_comm_api $(BIN_DIR)

all: build_dll

$(BIN_DIR):
	@echo ---------------------------
	@echo --- Creating directory ---
	@echo ---------------------------
	mkdir -p $@

build_dll: $(BIN_DIR) build_libs $(TARGET_DLL)

$(TARGET_DLL):  $(C_OBJS_EXT) $(CPP_OBJS_EXT)
	@echo ---------------------------
	@echo --- Creating a dll      ---
	@echo ---------------------------
	$(LINK) -shared -o $@ $^ $(LINKFLAGS) $(LIBS)
#	@echo ---------------------------
#	@echo --- Deleting *.o files  ---
#	@echo ---------------------------
#	$(RM) $(BIN_DIR)/*.o
	@echo ---------------------------
	@echo --- Copy libs/headers   ---
	@echo ---------------------------
	cp $(API_DIR)/*.h $(BIN_DIR)/.
	cp $(DC3_API_INC_DIR)/*.h $(BIN_DIR)/.
	cp $(DC3_API_GEN_SRC_DIR)/*.h $(BIN_DIR)
	cp $(DC3_API_SRC_DIR)/*.h $(BIN_DIR)

build_libs: build_DC3_api
	@echo -----------------------------------
	@echo --- Building libraries complete ---
	@echo -----------------------------------

build_DC3_api:
	@echo ------------------------------------
	@echo --- Building DC3 Comm API library ---
	@echo ------------------------------------
	$(TRACE_FLAG)cd $(DC3_API_DIR); make TARGET=$(TARGET) CONF=$(CONF) TRACE=$(TRACE) all
	
$(BIN_DIR)/%.d : %.c
	$(CPP) -MM -MT $(@:.d=.o) $(CPPFLAGS) $< > $@

$(BIN_DIR)/%.d : %.cpp
	$(CPP) -MM -MT $(@:.d=.o) $(CPPFLAGS) $< > $@

$(BIN_DIR)/%.o : %.c
	$(CPP) $(CPPFLAGS) -c $< -o $@

$(BIN_DIR)/%.o : %.cpp
	$(CPP) $(CPPFLAGS) -c $< -o $@

cleanall: clean_with_libs
	@echo ---------------------------
	@echo --- Cleaning EVERYTHING
	@echo ---------------------------
	$(RM) dbg_posix rel_posix_spy_posix dbg_win32 rel_win32 spy_win32
	
clean:
	$(RM) $(BIN_DIR)
	
clean_with_libs: clean clean_DC3_api_libs
	@echo -------------------------
	@echo --- Cleaned all libs ----
	@echo -------------------------

clean_DC3_api_libs:
	@echo -------------------------------
	@echo --- Cleaning DC3 API library ---
	@echo -------------------------------
	$(TRACE_FLAG)cd $(DC3_API_DIR); make TARGET=$(TARGET) TRACE=$(TRACE) cleanall

show:
	@echo CONF             = $(CONF)
	@echo C_SRCS           = $(C_SRCS)
	@echo C_OBJS_EXT       = $(C_OBJS_EXT)
	@echo C_DEPS_EXT       = $(C_DEPS_EXT)
	@echo CPP_SRCS         = $(CPP_SRCS)
	@echo CPP_DEPS_EXT     = $(CPP_DEPS_EXT)
	@echo LIBS             = $(LIBS)
	@echo LBITS            = $(LBITS)
	@echo CROSS            = $(CROSS)
	@echo EXT_LIB_DIR      = $(EXT_LIB_DIR)
	@echo BOOST_LIB_DIR    = $(BOOST_LIB_DIR)