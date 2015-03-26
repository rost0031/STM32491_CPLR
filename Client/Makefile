##############################################################################
# Product: Makefile for Redwood L1 CLI Client
#
#                             Datacard 
#                    ---------------------------
#
# Copyright (C) 2013 Datacard. All rights reserved.
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


# Output file basename (should be the same as the directory name)
PROJECT_NAME                = CPLR_CLI_Client$(EXT)
PROJECT_DLL                 = CPLR_Client

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
 
    EXT                     = .exe                 # Appended to the target name
    QPLIBMOD                = WIN32
#    BOOSTLIBMOD             = -mt-s
    BOOSTLIBMOD             = -mt.dll
    BOOST_TH_LIBMOD         = _$(TARGET)

else                    # Compile natively on Linux and/or Cygwin
    CROSS                   = 
    EXT                     =                      # Appended to the target name
    QPLIBMOD                = posix
    BOOSTLIBMOD             = -mt
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
SRC_DIR                     = ./src
COMMON_CLI_SRC_DIR          = ../Common

# External library includes
EXT_LIB_DIR                 = ./lib/$(TARGET)
EXT_LIB_BIN_DIR             = $(EXT_LIB_DIR)/lib
EXT_LIB_INC_DIR             = $(EXT_LIB_DIR)/inc

# app, bsp, and sys directories common between firmware and client
COMMON_CLI_BSP_DIR          = $(COMMON_CLI_SRC_DIR)/bsp
COMMON_CLI_SYS_DIR          = $(COMMON_CLI_SRC_DIR)/sys

# app, bsp, sys, dll, and api directories 
CMD_DIR                     = $(SRC_DIR)/app/cmd
GUI_DIR                     = $(SRC_DIR)/app/gui
API_DIR                     = $(SRC_DIR)/api
DLL_DIR                     = $(SRC_DIR)/dll
BSP_DIR                     = $(SRC_DIR)/bsp
SYS_DIR                     = $(SRC_DIR)/sys

# QPC directories
QPC                         = $(COMMON_CLI_SYS_DIR)/qpc_5.3.1
QP_PORT_INC_DIR             = $(QPC)/ports/$(TARGET)    
ifeq (win32, $(TARGET))      
    QP_PORT_DIR             = $(QPC)/ports/$(TARGET)/mingw
else
    QP_PORT_DIR             = $(QPC)/ports/$(TARGET)/gnu
endif

# Coupler Board API directory
CB_API_DIR                  = $(COMMON_CLI_SYS_DIR)/cb_api
CB_API_GEN_SRC_DIR          = $(CB_API_DIR)/gen_src
CB_API_SRC_DIR              = $(CB_API_DIR)/src
CB_API_INC_DIR              = $(CB_API_DIR)/inc
CB_API_LIB_DIR              = $(CB_API_DIR)/$(TARGET)_lib

# Boost library
BOOST_INC_DIR               = $(EXT_LIB_INC_DIR)
BOOST_LIB_DIR               = $(EXT_LIB_BIN_DIR)/boost

# Base64 encoding module
BASE64_DIR                  = $(COMMON_CLI_SYS_DIR)/libb64

# Shared Client Utils
FWLDR_DIR                   = $(SYS_DIR)/fwLdr

#-----------------------------------------------------------------------------
# SOURCE VIRTUAL DIRECTORIES
#-----------------------------------------------------------------------------
VPATH                       = $(API_DIR) \
                              $(CMD_DIR) \
                              $(GUI_DIR) \
                              $(DLL_DIR) \
                              $(BSP_DIR) \
                              $(SYS_DIR) \
                              $(BASE64_DIR) \
                              $(FWLDR_DIR)

#-----------------------------------------------------------------------------
# INCLUDE DIRECTORIES
#-----------------------------------------------------------------------------
INCLUDES                    = -I$(SRC_DIR) \
                              -I$(API_DIR) \
                              -I$(CMD_DIR) \
                              -I$(GUI_DIR) \
                              -I$(DLL_DIR) \
                              -I$(BSP_DIR) \
                              -I$(SYS_DIR) \
                              \
                              -I$(CB_API_SRC_DIR) \
                              -I$(CB_API_INC_DIR) \
                              -I$(CB_API_GEN_SRC_DIR) \
                              \
                              -I$(BASE64_DIR) \
                              -I$(FWLDR_DIR) \
                              \
                              -I$(QPC)/include \
                              -I$(QP_PORT_DIR) \
                              -I$(QP_PORT_INC_DIR) \
                              \
                              -I$(BOOST_INC_DIR) \
                              \
                              -I$(SYS_DIR) \
                              -I$(COMMON_CLI_SYS_DIR)

#-----------------------------------------------------------------------------
# DEFINES
#-----------------------------------------------------------------------------
DEFINES                     = -DDFUSE -DBUILDING_CLIENT_DLL

# Modify the INCLUDES to point to the boost includes compiled for windows
ifeq (win32, $(TARGET)) 
    
endif

#-----------------------------------------------------------------------------
# FILES
#-----------------------------------------------------------------------------

# assembler source files
ASM_SRCS                    = 

# C source files
C_SRCS                      = cdecode.c \
                              cencode.c \
                              base64_wrapper.c

# C++ source files
CPP_SRCS                    = bsp.cpp \
                              serial.cpp \
                              eth.cpp \
                              fwLdr.cpp
                           
API_CPP_SRCS                = ClientApi.cpp \
                              Job.cpp \
                              MainMgr.cpp

APP_CLI_CPP_SRCS            = main.cpp \
                              Callbacks.cpp
                              
                              
 
APP_GUI_CPP_SRCS            = main_test_api.cpp

#-----------------------------------------------------------------------------
# BUILD OPTIONS FOR VARIOUS CONFIGURATIONS
#-----------------------------------------------------------------------------
ifeq (rel, $(CONF))       # Release configuration ............................

    BIN_DIR                := rel
    DEFINES                += -DNDEBUG
    ASFLAGS                 = 
    CFLAGS                  = -c -std=gnu99 -Wall $(INCLUDES) $(DEFINES)

    CPPFLAGS                = -O2 -Wall -std=gnu++0x -c -Wall \
                              -Wno-write-strings $(INCLUDES) $(DEFINES)

else ifeq (spy, $(CONF))  # Spy configuration ................................

    BIN_DIR                := spy
    DEFINES                += -DQ_SPY
    ASFLAGS                 = 
    CFLAGS                  = -c -std=gnu99 -Wall \
                              -g $(INCLUDES) $(DEFINES)

    CPPFLAGS                = -O0 -g3 -Wall -std=gnu++0x -c -Wall \
                              -Wno-write-strings -g $(INCLUDES) $(DEFINES)

else                     # default Debug configuration .......................

    BIN_DIR                := dbg


    ASFLAGS                 = 
    CFLAGS                  = -c -g3 -ggdb -std=gnu99 -Wall \
                               $(INCLUDES) $(DEFINES)

    CPPFLAGS                = -O0 -g3 -ggdb  -std=gnu++0x -c -Wall \
                              -Wno-write-strings $(INCLUDES) $(DEFINES)
endif


# Modify the INCLUDES to point to the boost libraries compiled for windows
ifeq (win32, $(TARGET))
    LINKFLAGS               =  -Wl,--out-implib=$(TARGET_DLL).a \
                               -Wl,--export-all-symbols \
                               -Wl,--enable-auto-import \
                               -Wl,--whole-archive $(API_STATIC_LIBS) \
                               -Wl,--no-whole-archive $(STATIC_LIBS)
	    		
    LIBS                   += -lws2_32
    DEFINES                += -DBOOST_THREAD_USE_LIB -DWIN32COMPILE -DBOOST_ALL_NO_LIB -D_SECURE_SCL=0

  
    DLL_EXT                 = dll
    LIB_PREFIX              = lib
    DLL_LIBS                = -L$(BIN_DIR) -l$(LIB_PREFIX)$(PROJECT_DLL)
  
    DEP_GCC_DLL_CP_CMD      = cp /usr/$(CROSS_PATH)/sys-root/mingw/bin/libgcc_s_seh-1.dll $(BIN_DIR)/.
    DEP_STDC_DLL_CP_CMD     = cp /usr/$(CROSS_PATH)/sys-root/mingw/bin/libstdc++-6.dll $(BIN_DIR)/.
    
    API_STATIC_LIBS         = $(CB_API_LIB_DIR)/libCBCommApi_$(TARGET).a \
                              $(CB_API_LIB_DIR)/libprotobuf_core_$(TARGET).a \
                              $(BOOST_LIB_DIR)/libboost_program_options$(BOOSTLIBMOD).a \
                              $(BOOST_LIB_DIR)/libboost_system$(BOOSTLIBMOD).a \
                              $(BOOST_LIB_DIR)/libboost_thread$(BOOST_TH_LIBMOD)$(BOOSTLIBMOD).a
  
else
    LINKFLAGS               = -Wl,-soname,$(LIB_PREFIX)$(PROJECT_DLL).$(DLL_EXT) \
                              -Wl,--whole-archive $(API_STATIC_LIBS) \
                              -Wl,--no-whole-archive $(STATIC_LIBS)
                          
    API_STATIC_LIBS         = $(CB_API_LIB_DIR)/libCBCommApi_$(TARGET).a \
                              $(CB_API_LIB_DIR)/libprotobuf_core_$(TARGET).a
                            
    LIBS                   += -lpthread  # This is needed on newer Ubuntu systems but doesn't hurt older ones or Cygwin
  
    DLL_EXT                 = so
    LIB_PREFIX              = lib
    DLL_LIBS                = -L$(BIN_DIR) -l$(PROJECT_DLL)
  
    DEP_GCC_DLL_CP_CMD      = cp $(BOOST_LIB_DIR)/libboost_program_options$(BOOSTLIBMOD).$(DLL_EXT) $(BIN_DIR)/.; \
                              cp $(BOOST_LIB_DIR)/libboost_system$(BOOSTLIBMOD).$(DLL_EXT) $(BIN_DIR)/.; \
                              cp $(BOOST_LIB_DIR)/libboost_thread$(BOOST_TH_LIBMOD)$(BOOSTLIBMOD).$(DLL_EXT) $(BIN_DIR)/. 
    DEP_STDC_DLL_CP_CMD     =

    CFLAGS                 += -pthread -fPIC
    CPPFLAGS               += -pthread -fPIC
endif


TARGET_EXE                  = $(BIN_DIR)/$(PROJECT_NAME)
TARGET_API_TEST             = $(BIN_DIR)/ApiTest$(EXT)
TARGET_DLL                  = $(BIN_DIR)/$(LIB_PREFIX)$(PROJECT_DLL).$(DLL_EXT)


                              
STATIC_LIBS                 = $(QP_PORT_DIR)/$(BIN_DIR)/libqp_$(QPLIBMOD)_cs.a
                              

ASM_OBJS                    = $(patsubst %.S,%.o,$(ASM_SRCS))
C_OBJS                      = $(patsubst %.c,%.o,$(C_SRCS))
CPP_OBJS                    = $(patsubst %.cpp,%.o,$(CPP_SRCS))
CPP_OBJS                    = $(patsubst %.cpp,%.o,$(CPP_SRCS))
API_CPP_OBJS                = $(patsubst %.cpp,%.o,$(API_CPP_SRCS))
APP_CLI_CPP_OBJS            = $(patsubst %.cpp,%.o,$(APP_CLI_CPP_SRCS))
APP_GUI_CPP_OBJS            = $(patsubst %.cpp,%.o,$(APP_GUI_CPP_SRCS))

ASM_OBJS_EXT                = $(addprefix $(BIN_DIR)/, $(ASM_OBJS))
C_OBJS_EXT                  = $(addprefix $(BIN_DIR)/, $(C_OBJS))
C_DEPS_EXT                  = $(patsubst %.o, %.d, $(C_OBJS_EXT))
CPP_OBJS_EXT                = $(addprefix $(BIN_DIR)/, $(CPP_OBJS))
CPP_DEPS_EXT                = $(patsubst %.o, %.d, $(CPP_OBJS_EXT))
API_CPP_OBJS_EXT            = $(addprefix $(BIN_DIR)/, $(API_CPP_OBJS))
API_CPP_DEPS_EXT            = $(patsubst %.o, %.d, $(API_CPP_OBJS_EXT))
APP_CLI_CPP_OBJS_EXT        = $(addprefix $(BIN_DIR)/, $(APP_CLI_CPP_OBJS))
APP_CLI_CPP_DEPS_EXT        = $(patsubst %.o, %.d, $(APP_CLI_CPP_OBJS_EXT))
APP_GUI_CPP_OBJS_EXT        = $(addprefix $(BIN_DIR)/, $(APP_GUI_CPP_OBJS))
APP_GUI_CPP_DEPS_EXT        = $(patsubst %.o, %.d, $(APP_GUI_CPP_OBJS_EXT))

#-----------------------------------------------------------------------------
# BUILD TARGETS
#-----------------------------------------------------------------------------

.PHONY: build_libs build_dll all api_test clean clean_with_libs build_comm build_qpc build_cb_comm_api $(BIN_DIR)

all:  $(TARGET_EXE)

api_test: $(TARGET_API_TEST)

$(BIN_DIR):
	@echo ---------------------------
	@echo --- Creating directory ---
	@echo ---------------------------
	mkdir -p $@

$(TARGET_API_TEST): build_dll $(APP_GUI_CPP_OBJS_EXT)
	@echo ---------------------------
	@echo --- Building API Test   ---
	@echo ---------------------------
	$(LINK) -o $(BIN_DIR)/ApiTest$(EXT) $(APP_GUI_CPP_OBJS_EXT) $(DLL_LIBS)
	$(RM) $(BIN_DIR)/*.o
	
$(TARGET_EXE): build_dll $(APP_CLI_CPP_OBJS_EXT) 
	@echo ---------------------------
	@echo --- Building CMD Client ---
	@echo ---------------------------
	$(LINK) -o $(TARGET_EXE) $(APP_CLI_CPP_OBJS_EXT) $(DLL_LIBS)
	$(RM) $(BIN_DIR)/*.o
    
build_dll: $(BIN_DIR) build_libs $(TARGET_DLL)

$(TARGET_DLL):  $(C_OBJS_EXT) $(CPP_OBJS_EXT) $(API_CPP_OBJS_EXT) 
	@echo ---------------------------
	@echo --- Creating a dll      ---
	@echo ---------------------------
	$(LINK) -shared -o $@ $^ $(LINKFLAGS) $(LIBS)
	@echo ---------------------------
	@echo --- Deleting *.o files  ---
	@echo ---------------------------
	$(RM) $(BIN_DIR)/*.o
	@echo ---------------------------
	@echo --- Copy libs/headers   ---
	@echo ---------------------------
	$(DEP_GCC_DLL_CP_CMD)
	$(DEP_STDC_DLL_CP_CMD)
	@echo --- ClientApi headers   ---
	cp $(API_DIR)/*.h $(BIN_DIR)/.

build_libs: build_qpc build_cb_api
	@echo -----------------------------------
	@echo --- Building libraries complete ---
	@echo -----------------------------------

build_qpc:
	@echo ------------------------------
	@echo --- Building QPC libraries ---
	@echo ------------------------------
	cd $(QP_PORT_DIR); make CONF=$(BIN_DIR)

build_cb_api:
	@echo ------------------------------------
	@echo --- Building CB Comm API library ---
	@echo ------------------------------------
	$(TRACE_FLAG)cd $(CB_API_DIR); make TARGET=$(TARGET) TRACE=$(TRACE) all
	
$(BIN_DIR)/%.d : %.c
	$(CPP) -MM -MT $(@:.d=.o) $(CPPFLAGS) $< > $@

$(BIN_DIR)/%.d : %.cpp
	$(CPP) -MM -MT $(@:.d=.o) $(CPPFLAGS) $< > $@

$(BIN_DIR)/%.o : %.S
	@echo ---------------------------
	@echo --- Compiling ASM object ---
	@echo ---------------------------
	$(AS) $(ASFLAGS) $< -o $@

$(BIN_DIR)/%.o : %.c
	$(CPP) $(CPPFLAGS) -c $< -o $@

# This is overkill, saying all .o's depend on all .h's.
$(BIN_DIR)/%.o : %.cpp $(DLL_DIR)/*.h
	$(CPP) $(CPPFLAGS) -c $< -o $@

cleanall: clean_with_libs
	@echo ---------------------------
	@echo --- Cleaning EVERYTHING
	@echo ---------------------------

	$(RM) dbg 
	$(RM) spy 
	$(RM) rel 
	
clean:
	$(RM) $(BIN_DIR)/*.o \
	$(BIN_DIR)/*.d \
	$(BIN_DIR)/*.exe \
	$(BIN_DIR)/*.dll \
	$(BIN_DIR)/*.a \
	$(BIN_DIR)/*.h
	
clean_with_libs: clean clean_qpc_libs clean_cb_api_libs
	@echo -------------------------
	@echo --- Cleaned all libs ----
	@echo -------------------------
	
clean_qpc_libs:
	@echo ----------------------------
	@echo --- Cleaning QPC library ---
	@echo ----------------------------
	cd $(QP_PORT_DIR); make CONF=dbg clean
	cd $(QP_PORT_DIR); make CONF=spy clean
	cd $(QP_PORT_DIR); make CONF=rel clean

clean_cb_api_libs:
	@echo -------------------------------
	@echo --- Cleaning CB API library ---
	@echo -------------------------------
	$(TRACE_FLAG)cd $(CB_API_DIR); make TARGET=$(TARGET) TRACE=$(TRACE) cleanall

show:
	@echo CONF             = $(CONF)
	@echo ASM_SRCS         = $(ASM_SRCS)
	@echo C_SRCS           = $(C_SRCS)
	@echo CPP_SRCS         = $(CPP_SRCS)
	@echo ASM_OBJS_EXT     = $(ASM_OBJS_EXT)
	@echo C_OBJS_EXT       = $(C_OBJS_EXT)
	@echo C_DEPS_EXT       = $(C_DEPS_EXT)
	@echo CPP_DEPS_EXT     = $(CPP_DEPS_EXT)
	@echo TARGET_EXE       = $(TARGET_EXE)
	@echo LIBS             = $(LIBS)
	@echo LBITS            = $(LBITS)
	@echo CROSS            = $(CROSS)
	@echo API_CPP_OBJS     = $(API_CPP_OBJS)
	@echo APP_CLI_CPP_OBJS = $(APP_CLI_CPP_OBJS)
	@echo API_CPP_OBJS_EXT = $(API_CPP_OBJS_EXT)
	@echo API_CPP_DEPS_EXT = $(API_CPP_DEPS_EXT)
	@echo APP_CPP_OBJS_EXT = $(APP_CPP_OBJS_EXT)
	@echo APP_CPP_DEPS_EXT = $(APP_CPP_DEPS_EXT)
	@echo EXT_LIB_DIR      = $(EXT_LIB_DIR)
	@echo BOOST_LIB_DIR    = $(BOOST_LIB_DIR)