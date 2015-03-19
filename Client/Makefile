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

    # Append a .exe to the target name
    EXT                    := .exe
	
    # This will detect whether the linux system is 64 or 32 bit since the mingw 
    # compiler's name is dependent on it
    LBITS                  := $(shell getconf LONG_BIT)
    ifeq ($(LBITS),64)
        CROSS_PATH         := x86_64-w64-mingw32
        CROSS              := $(CROSS_PATH)-
    else
        CROSS_PATH         := i686-w64-mingw32
        CROSS              := $(CROSS_PATH)-
    endif

    CC                      = $(CROSS)g++
    CPP                     = $(CROSS)g++
    AS                      = $(CROSS)as
    LINK                    = $(CROSS)g++
    OBJCPY                  = $(CROSS)objcopy
    RANLIB					= $(CROSS)ranlib
    QPLIBMOD                = WIN32
    BOOSTLIBMOD             = -s
    BOOST_TH_LIBMOD         = _$(TARGET)

else                    # Compile natively on Linux and/or Cygwin
    EXT                     = 
    CC                      = g++
    CPP                     = g++
    AS                      = as
    LINK                    = g++
    OBJCPY                  = objcopy
    QPLIBMOD                = POSIX
    BOOSTLIBMOD             =
    BOOST_TH_LIBMOD         = 
    TARGET                  = ia32
endif

#------------------------------------------------------------------------------
#  TOOLCHAIN SETUP - Common for all targets
#------------------------------------------------------------------------------
RM                          = rm -rf
ECHO                        = echo
MKDIR                       = mkdir

#-----------------------------------------------------------------------------
# DIRECTORIES
#-----------------------------------------------------------------------------
SRC_DIR                     = ./src
CMD_DIR                     = $(SRC_DIR)/app/cmd
GUI_DIR                     = $(SRC_DIR)/app/gui
API_DIR                     = $(SRC_DIR)/api
DLL_DIR                     = $(SRC_DIR)/dll
BSP_DIR                     = $(SRC_DIR)/bsp
SYS_DIR                     = $(SRC_DIR)/sys

# QPC directories
QPC                         = $(SYS_DIR)/qpc_shared

ifeq (win32, $(TARGET)) 
    QP_PORT_DIR             = $(QPC)/ports/80x86/win32/mingw
else
    QP_PORT_DIR             = $(QPC)/ports/80x86/posix/gnu
endif

# RFID API directory
RFID_API_DIR                = $(SYS_DIR)/rfid_api_shared
RFID_API_GEN_SRC            = $(RFID_API_DIR)/gen_src
RFID_API_SRC                = $(RFID_API_DIR)/src
RFID_API_INC                = $(RFID_API_DIR)/inc
RFID_API_LIB                = $(RFID_API_DIR)/ia32_lib

# CommStack directories
COMM_DIR                    = $(SYS_DIR)/msgs_shared
COMM_INC_DIR                = $(COMM_DIR)/gen_src
COMM_ERR_DIR                = $(COMM_DIR)/include
COMM_LIB_DIR                = $(COMM_DIR)/ia32_lib

# Base64 encoding module
BASE64_DIR                  = $(SYS_DIR)/libb64_shared

# Shared Client Utils
UTILS_SHARED_DIR            = $(SYS_DIR)/utils_shared
FWLDR_DIR                   = $(UTILS_SHARED_DIR)/fwLdr

#-----------------------------------------------------------------------------
# SOURCE VIRTUAL DIRECTORIES
#-----------------------------------------------------------------------------
VPATH                       = $(API_DIR) \
                              $(CMD_DIR) \
                              $(GUI_DIR) \
                              $(DLL_DIR) \
                              $(BSP_DIR) \
                              $(COMM_SM) \
                              $(BASE64_DIR) \
                              $(FWLDR_DIR) \
                              $(RFID_API_SRC) \
                              $(SYS_DIR) \
                              $(SYS_DIR)/rfid_msgs \
                              $(SYS_DIR)/utils_shared/cmd

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
                              -I$(COMM_INC_DIR) \
                              -I$(COMM_ERR_DIR) \
                              \
                              -I$(RFID_API_SRC) \
                              -I$(RFID_API_INC) \
                              -I$(RFID_API_GEN_SRC) \
                              \
                              -I$(BASE64_DIR) \
                              -I$(FWLDR_DIR) \
                              \
                              -I$(QPC)/include \
                              -I$(QP_PORT_DIR) \
                              \
                              -I$(SYS_DIR) \
                              -I$(SYS_DIR)/rfid_msgs \
                              -I$(SYS_DIR)/utils_shared/cmd

#-----------------------------------------------------------------------------
# DEFINES
#-----------------------------------------------------------------------------
DEFINES                     = -DDFUSE

# Modify the INCLUDES to point to the boost includes compiled for windows
ifeq (win32, $(TARGET)) 
    INCLUDES               += -I/opt/boost_1.54_mingw/include
    DEFINES                += -DBUILDING_CLIENT_DLL
endif

#-----------------------------------------------------------------------------
# FILES
#-----------------------------------------------------------------------------

# assembler source files
ASM_SRCS                   := 

# C source files
C_SRCS                     := cdecode.c \
                              cencode.c \
                              dfuse.c \
                              base64_wrapper.c \
                              dfuse_err_convert.c \
                              rfid_msg_iface.c

# C++ source files
CPP_SRCS                   := bsp.cpp \
                              \
                              CommStackMgr.cpp \
                              \
                              MsgUtils.cpp \
                              CmdlineParser.cpp \
                              SettingsFiles.cpp \
                              serial.cpp \
                              eth.cpp \
                              fwLdr.cpp
                           
API_CPP_SRCS               := ClientApi.cpp \
                              ParamMaps.cpp \
                              RedwoodMsg.cpp

APP_CLI_CPP_SRCS           := Callbacks.cpp main.cpp 
APP_GUI_CPP_SRCS           := main_test_api.cpp

#-----------------------------------------------------------------------------
# BUILD OPTIONS FOR VARIOUS CONFIGURATIONS
#-----------------------------------------------------------------------------
ifeq (rel, $(CONF))       # Release configuration ............................

    BIN_DIR                := rel
    DEFINES                += -DNDEBUG
    ASFLAGS                 = 
    CFLAGS                  = -c -std=gnu99 -Wall $(INCLUDES) $(DEFINES)

    CPPFLAGS                = -O2 -Wall -std=gnu++0x -c -Wall -Wno-write-strings \
                              $(INCLUDES) $(DEFINES)

else ifeq (spy, $(CONF))  # Spy configuration ................................

    BIN_DIR                := spy
    DEFINES                += -DQ_SPY
    ASFLAGS                 = 
    CFLAGS                  = -c -std=gnu99 -Wall \
                              -g $(INCLUDES) $(DEFINES)

    CPPFLAGS                = -O0 -g3 -Wall -std=gnu++0x -c -Wall -Wno-write-strings \
                              -g $(INCLUDES) $(DEFINES)

else                     # default Debug configuration .......................

    BIN_DIR                := dbg


    ASFLAGS                 = 
    CFLAGS                  = -c -std=gnu99 -Wall -g $(INCLUDES) $(DEFINES)

    CPPFLAGS                = -O0 -g3 -Wall -std=gnu++0x -c -Wall -Wno-write-strings \
                              -g $(INCLUDES) $(DEFINES)
endif


# Modify the INCLUDES to point to the boost libraries compiled for windows
ifeq (win32, $(TARGET))
    LIB_BOOST_LOC           = /opt/boost_1.54_mingw/lib
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
  
    DEP_GCC_DLL_CP_CMD      = cp /usr/lib/gcc/$(CROSS_PATH)/4.6/libgcc_s_sjlj-1.dll $(BIN_DIR)/.
    DEP_STDC_DLL_CP_CMD     = cp /usr/lib/gcc/$(CROSS_PATH)/4.6/libstdc++-6.dll $(BIN_DIR)/.
  
else
    LIB_BOOST_LOC           = /usr/lib/
    LINKFLAGS               = -Wl,-soname,$(LIB_PREFIX)$(PROJECT_DLL).$(DLL_EXT) \
                              -Wl,--export-all-symbols \
                              -Wl,--whole-archive $(API_STATIC_LIBS) \
                              -Wl,--no-whole-archive $(STATIC_LIBS)
                             
    LIBS                   += -lpthread  # This is needed on newer Ubuntu systems but doesn't hurt older ones or Cygwin
  
    DLL_EXT                 = so
    LIB_PREFIX              = lib
    DLL_LIBS                = -L$(BIN_DIR) -l$(PROJECT_DLL)
  
    DEP_GCC_DLL_CP_CMD      =
    DEP_STDC_DLL_CP_CMD     =
endif


TARGET_EXE                  = $(BIN_DIR)/$(PROJECT_NAME)
TARGET_API_TEST             = $(BIN_DIR)/ApiTest$(EXT)
TARGET_DLL                  = $(BIN_DIR)/$(LIB_PREFIX)$(PROJECT_DLL).$(DLL_EXT)

API_STATIC_LIBS				= $(RFID_API_LIB)/librfid_api_$(TARGET).a \
                              $(COMM_LIB_DIR)/libredwood_api_$(TARGET).a \
                              $(COMM_LIB_DIR)/libprotobuf_core_$(TARGET).a \
                              $(LIB_BOOST_LOC)/libboost_program_options-mt$(BOOSTLIBMOD).a \
                              $(LIB_BOOST_LOC)/libboost_system-mt$(BOOSTLIBMOD).a \
                              $(LIB_BOOST_LOC)/libboost_thread$(BOOST_TH_LIBMOD)-mt$(BOOSTLIBMOD).a \
                              
                              
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

.PHONY: build_libs build_dll all api_test clean clean_with_libs build_comm build_qpc build_rfid_api $(BIN_DIR)

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
	@echo --- Redwood API headers ---
	cp $(COMM_INC_DIR)/redwood_msgs.h $(BIN_DIR)/.
	cp $(COMM_ERR_DIR)/*.h $(BIN_DIR)/.
	@echo --- RFID board API headers ---
	cp $(RFID_API_GEN_SRC)/rfid_board_api.h $(BIN_DIR)/.
	cp $(RFID_API_INC)/RFID_Error.h $(BIN_DIR)/.

build_libs: build_comm build_qpc build_rfid_api
	@echo -----------------------------------
	@echo --- Building libraries complete ---
	@echo -----------------------------------

build_qpc:
	@echo ------------------------------
	@echo --- Building QPC libraries ---
	@echo ------------------------------
	cd $(QP_PORT_DIR); make CONF=$(BIN_DIR)

build_comm:
	@echo --------------------------------
	@echo --- Building COMM libraries  ---
	@echo --------------------------------
	cd $(COMM_DIR); make TARGET=$(TARGET) all

build_rfid_api:
	@echo ---------------------------------
	@echo --- Building RFID API library ---
	@echo ---------------------------------
	$(TRACE_FLAG)cd $(RFID_API_DIR); make TARGET=$(TARGET) TRACE=$(TRACE) all
	
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

	$(RM) dbg/*.o dbg/*.d dbg/*.hex dbg/*.elf dbg/*.map dbg
	$(RM) spy/*.o spy/*.d spy/*.hex spy/*.elf spy/*.map rel
	$(RM) rel/*.o rel/*.d rel/*.hex rel/*.elf rel/*.map spy
	
clean:
	$(RM) $(BIN_DIR)/*.o \
	$(BIN_DIR)/*.d \
	$(BIN_DIR)/*.exe \
	$(BIN_DIR)/*.dll \
	$(BIN_DIR)/*.a \
	$(BIN_DIR)/*.h
	
clean_with_libs: clean clean_qpc_libs clean_redwood_api_libs clean_rfid_api_libs
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

clean_redwood_api_libs:
	@echo ------------------------------------
	@echo --- Cleaning Redwood API library ---
	@echo ------------------------------------
	$(TRACE_FLAG)make -C $(COMM_DIR) cleanall

clean_rfid_api_libs:
	@echo ---------------------------------
	@echo --- Cleaning RFID API library ---
	@echo ---------------------------------
	$(TRACE_FLAG)make -C $(RFID_API_DIR) cleanall

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
