##############################################################################
# Product: Makefile for DC3 Coupler Board Command Line Client 
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
PROJECT_NAME                = DC3_CmdLineClient$(EXT)
PROJECT_DLL                 = DC3Client
BOOST_LIB_VER               = 1.57.0

#------------------------------------------------------------------------------
#  TOOLCHAIN SETUP - Depends on TARGET
#------------------------------------------------------------------------------
ifeq (posix, $(TARGET)) # Compile natively on Linux and/or Cygwin  
    CROSS                   = 
    EXT                     =                      # Appended to the target name
    QPLIBMOD                = posix
    BOOSTLIBMOD             = -mt-s
    BOOST_TH_LIBMOD         = 
    TARGET                  = posix
else          # Cross compile for win32 from Linux (or from Cygwin64 with Mingw)

    # explicitly define target since it's used for naming files
    TARGET                  = win32
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
    BOOSTLIBMOD             = -mt-s
    BOOST_TH_LIBMOD         = _$(TARGET)
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

# External library includes
EXT_LIB_DIR                 = ../../lib
EXT_LIB_BIN_DIR             = $(EXT_LIB_DIR)/$(TARGET)
EXT_LIB_INC_DIR             = $(EXT_LIB_DIR)/inc

# Boost library
BOOST_INC_DIR               = $(EXT_LIB_INC_DIR)
BOOST_LIB_DIR               = $(EXT_LIB_BIN_DIR)

# Shared Client library directories
SHARED_LIB_DIR              = ../shared_lib_src
SHARED_LIB_BIN_DIR          = $(SHARED_LIB_DIR)/$(BIN_DIR)

#-----------------------------------------------------------------------------
# SOURCE VIRTUAL DIRECTORIES
#-----------------------------------------------------------------------------
VPATH                       = $(SRC_DIR)
                              
#-----------------------------------------------------------------------------
# INCLUDE DIRECTORIES
#-----------------------------------------------------------------------------
INCLUDES                    = -I$(SRC_DIR) \
                              -I$(BOOST_INC_DIR) \
                              -I$(BIN_DIR)
                              
#-----------------------------------------------------------------------------
# DEFINES
#-----------------------------------------------------------------------------
DEFINES                     = 

#-----------------------------------------------------------------------------
# FILES
#-----------------------------------------------------------------------------

# C++ source files
CPP_SRCS                    = main.cpp \
                              Callbacks.cpp \
                              Help.cpp \
                              EnumMaps.cpp \
                              Logging.cpp \
                              KTree.cpp \
                              Menu.cpp \
                              Cmds.cpp \
                              ArgParse.cpp
                              
#-----------------------------------------------------------------------------
# BUILD OPTIONS FOR VARIOUS CONFIGURATIONS
#-----------------------------------------------------------------------------
ifeq (rel, $(CONF))       # Release configuration ............................

    BIN_DIR                := rel_$(TARGET)
    DEFINES                += -DNDEBUG
    CFLAGS                  = -c -std=gnu99 -Wall $(INCLUDES) $(DEFINES)

    CPPFLAGS                = -O2 -Wall -std=gnu++11 -c -Wall \
                              -Wno-write-strings $(INCLUDES) $(DEFINES)

else ifeq (spy, $(CONF))  # Spy configuration ................................

    BIN_DIR                := spy_$(TARGET)
    DEFINES                += -DQ_SPY
    CFLAGS                  = -c -std=gnu99 -Wall \
                              -g $(INCLUDES) $(DEFINES)

    CPPFLAGS                = -O0 -g3 -Wall -std=gnu++11 -c -Wall \
                              -Wno-write-strings -g $(INCLUDES) $(DEFINES)

else                     # default Debug configuration .......................
    
    BIN_DIR                := dbg_$(TARGET)
    CONF                    = dbg # Explicitly set the conf since it's used
    CFLAGS                  = -c -g3 -ggdb -std=gnu99 -Wall \
                               $(INCLUDES) $(DEFINES)

    CPPFLAGS                = -O0 -g3 -ggdb  -std=gnu++11 -c -Wall \
                              -Wno-write-strings $(INCLUDES) $(DEFINES)
endif
                              
# Modify the INCLUDES to point to the boost libraries compiled for windows
ifeq (win32, $(TARGET))

    DYNAMIC_LINKFLAGS       = -Wl,-Bdynamic 
    DYNAMIC_LIBS            = -l$(PROJECT_DLL) -lws2_32
    
    STATIC_LINKFLAGS        = -Wl,-Bstatic 
#    STATIC_LIBS             = -lstdc++ \
#                              -lboost_program_options$(BOOSTLIBMOD) \
#                              -lboost_date_time$(BOOSTLIBMOD)

    STATIC_LIBS             = -lstdc++ \
                              -lboost_program_options \
                              -lboost_date_time$(BOOSTLIBMOD) \
                              -lboost_log
    
    LIB_LOCATIONS           = -L$(BOOST_LIB_DIR) -L$(BIN_DIR)

    LIBS                   += -static-libgcc -static-libstdc++
    DEFINES                += -DBOOST_THREAD_USE_LIB -DWIN32COMPILE -DBOOST_ALL_NO_LIB -D_SECURE_SCL=0

    DLL_EXT                 = dll
    LIB_PREFIX              = lib
 
else
    DYNAMIC_LINKFLAGS       = -fPIC -Wl,-rpath -Wl,\$$ORIGIN -Wl,-Bdynamic -lpthread
    DYNAMIC_LIBS            = -l$(PROJECT_DLL) 
    
    STATIC_LINKFLAGS        = -Wl,-Bstatic 
    STATIC_LIBS             = -lboost_program_options$(BOOSTLIBMOD) \
                              -lboost_date_time$(BOOSTLIBMOD) -lboost_log

    LIB_LOCATIONS           = -L$(BOOST_LIB_DIR) -L$(BIN_DIR)
    LIBS                   += # Nothing needed for posix builds
                     
    DLL_EXT                 = so  
    LIB_PREFIX              = lib

endif

TARGET_EXE                  = $(BIN_DIR)/$(PROJECT_NAME)
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

.PHONY: build_dll all clean clean_with_libs build_DC3_comm_api $(BIN_DIR)

all:  $(TARGET_EXE)

$(BIN_DIR):
	@echo ---------------------------
	@echo --- Creating directory ---
	@echo ---------------------------
	mkdir -p $@

$(TARGET_EXE): build_dll $(CPP_OBJS_EXT) $(C_OBJS_EXT)
	@echo ---------------------------
	@echo --- Building CMD Client ---
	@echo ---------------------------
	$(LINK) $(LIBS) -o $(TARGET_EXE) $(CPP_OBJS_EXT) $(LIB_LOCATIONS) $(STATIC_LINKFLAGS) $(STATIC_LIBS) $(DYNAMIC_LINKFLAGS) $(DYNAMIC_LIBS)  
	#$(RM) $(BIN_DIR)/*.o
    
build_dll: $(BIN_DIR)
	@echo ---------------------------
	@echo --- Building client dll ---
	@echo ---------------------------
	cd $(SHARED_LIB_DIR); make TARGET=$(TARGET) CONF=$(CONF) TRACE=$(TRACE) all
	@echo --------------------------------------
	@echo --- Copying client dll and headers ---
	@echo --------------------------------------
	cp $(SHARED_LIB_BIN_DIR)/*.* $(BIN_DIR)/.
		
$(BIN_DIR)/%.d : %.c
	$(CPP) -MM -MT $(@:.d=.o) $(CPPFLAGS) $< > $@

$(BIN_DIR)/%.d : %.cpp
	$(CPP) -MM -MT $(@:.d=.o) $(CPPFLAGS) $< > $@

$(BIN_DIR)/%.o : %.c
	$(CPP) $(CPPFLAGS) -c $< -o $@

$(BIN_DIR)/%.o : %.cpp
	$(CPP) $(CPPFLAGS) -c $< -o $@

cleanall: clean_libs
	@echo ---------------------------
	@echo --- Cleaning EVERYTHING
	@echo ---------------------------
	$(RM) dbg_posix dbg_win32 spy_posix spy_win32 rel_posix rel_win32
	
clean:
	$(RM) $(BIN_DIR)
	
clean_libs:
	@echo ----------------------------
	@echo --- Cleaning DC3 library ---
	@echo ----------------------------
	cd $(SHARED_LIB_DIR); make TARGET=$(TARGET) CONF=$(CONF) TRACE=$(TRACE) cleanall

show:
	@echo CONF             = $(CONF)
	@echo C_SRCS           = $(C_SRCS)
	@echo C_OBJS_EXT       = $(C_OBJS_EXT)
	@echo C_DEPS_EXT       = $(C_DEPS_EXT)
	@echo CPP_SRCS         = $(CPP_SRCS)
	@echo CPP_DEPS_EXT     = $(CPP_DEPS_EXT)
	@echo TARGET_EXE       = $(TARGET_EXE)
	@echo LIBS             = $(LIBS)
	@echo LBITS            = $(LBITS)
	@echo CROSS            = $(CROSS)
	@echo EXT_LIB_DIR      = $(EXT_LIB_DIR)
	@echo BOOST_LIB_DIR    = $(BOOST_LIB_DIR)