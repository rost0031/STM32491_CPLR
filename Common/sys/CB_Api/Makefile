# If TRACE=0 -->TRACE_FLAG=
# If TRACE=1 -->TRACE_FLAG=@
# If TRACE=something -->TRACE_FLAG=something
TRACEON=$(TRACE:0=@)
TRACE_FLAG=$(TRACEON:1=)

RM              = rm -rf
ECHO            = echo
MKDIR           = mkdir -p

#------------------------------------------------------------------------------
#  CHECK TARGET
#------------------------------------------------------------------------------
ifeq (ppc, $(TARGET)) 

#------------------------------------------------------------------------------
#  C/C++ TOOLCHAIN SETUP FOR POWER PC
#------------------------------------------------------------------------------
CROSS           = $(PPC_TOOL)
CC              = $(CROSS)gcc
CPP             = $(CROSS)g++
AS              = $(CROSS)as
LINK            = $(CROSS)gcc
OBJCPY          = ${CROSS}objcopy
AR              = $(CROSS)ar
RANLIB			= $(CROSS)ranlib

# Build options
CFLAGS 			= -O2 -mcpu=e300c3 -fomit-frame-pointer -pipe \
				  -g -O $(INCLUDES) $(DEFINES)

LIBDIR			= ppc_lib
LIBFLAGS 		= rcs

else 
ifeq (arm, $(TARGET))

#------------------------------------------------------------------------------
#  C/C++ TOOLCHAIN SETUP FOR ARM CORTEX M3
#------------------------------------------------------------------------------
CROSS           = arm-none-eabi-
CC              = $(CROSS)gcc
CPP             = $(CROSS)g++
AS              = $(CROSS)as
LINK            = $(CROSS)gcc
OBJCPY          = ${CROSS}objcopy
AR              = $(CROSS)ar
RANLIB			= $(CROSS)ranlib

# Build options
ARM_CORE        = cortex-m3

CFLAGS          = -mcpu=$(ARM_CORE) -std=gnu99 -mthumb -Wall \
				  -ffunction-sections -fdata-sections \
				  -g -Os $(INCLUDES) $(DEFINES)

LIBDIR          = arm_lib
LIBFLAGS        = rcs

else 
ifeq (win32, $(TARGET)) 
#------------------------------------------------------------------------------
#  C/C++ TOOLCHAIN SETUP FOR WIN32
#------------------------------------------------------------------------------
# This will detect whether the linux system is 64 or 32 bit since the mingw 
# compiler's name is dependent on it
LBITS                  := $(shell getconf LONG_BIT)
ifeq ($(LBITS),64)
   CROSS                   = x86_64-w64-mingw32-
else
   CROSS                   = i686-w64-mingw32-
endif

CC              = $(CROSS)gcc
CPP             = $(CROSS)g++
AS              = $(CROSS)as
LINK            = $(CROSS)gcc
OBJCPY          = ${CROSS}objcopy
AR              = $(CROSS)ar
RANLIB			= $(CROSS)ranlib

# Build options
ARM_CORE 		= win32

CFLAGS 			= -std=gnu99 -Wall \
				  -ffunction-sections -fdata-sections \
				  -g -Os $(INCLUDES) $(DEFINES)

LIBDIR			= ia32_lib
LIBFLAGS 		= rcs

LAMGENPARAM     =
else #ia32 or none specified
#------------------------------------------------------------------------------
#  C/C++ TOOLCHAIN SETUP ia32 and ia64 (default)
#------------------------------------------------------------------------------
CC              = gcc
CPP             = g++
AS              = as
LINK            = gcc
OBJCPY          = objcopy
AR              = ar
RANLIB			= $(CROSS)ranlib

# Build options
CFLAGS          = -std=gnu99 -Wall \
                  -ffunction-sections -fdata-sections \
                  -g -Os $(INCLUDES) $(DEFINES)

LIBDIR          = ia32_lib
LIBFLAGS        = rcs

endif
endif
endif
#------------------------------------------------------------------------------
#  Which proto files to use.
#------------------------------------------------------------------------------
PROTO_FILES     = rfid_board_api.proto 

#------------------------------------------------------------------------------
# Files
#------------------------------------------------------------------------------
SRC_FILES       = dfuse.c \
                  util.c

# C source files (will be generated or already there)
C_SRCS 			= $(patsubst %.proto, %.c, $(PROTO_FILES)) $(SRC_FILES)
C_CORE_SRCS		= protobuf_core.c

#------------------------------------------------------------------------------
#  Common build options
#------------------------------------------------------------------------------
C_OBJS          = $(patsubst %.c, %.o, $(C_SRCS))
C_OBJS_EXT      = $(addprefix $(BIN_DIR)/, $(C_OBJS))
C_DEPS_EXT      = $(patsubst %.o, %.d, $(C_OBJS_EXT))

C_CORE_OBJS     = $(patsubst %.c, %.o, $(C_CORE_SRCS))
C_CORE_OBJS_EXT = $(addprefix $(BIN_DIR)/, $(C_CORE_OBJS))
C_CORE_DEPS_EXT = $(patsubst %.o, %.d, $(C_CORE_OBJS_EXT))

# defines
DEFINES         = 	

#------------------------------------------------------------------------------
# Directories
#------------------------------------------------------------------------------
PROTO_FILES_DIR	= proto
GEN_SOURCES_DIR	= gen_src
CMPLED_OBJS_DIR	= obj

SOURCES_DIR		= src

# source directories
VPATH	 	 	= $(GEN_SOURCES_DIR) $(SOURCES_DIR)

# include directories
INCLUDES     	= -I$(GEN_SOURCES_DIR) -I$(SOURCES_DIR)

# Where the objects will go
BIN_DIR 		= $(CMPLED_OBJS_DIR)

LIB_NAME    	= librfid_api_$(TARGET)
LIB_CORE		= libprotobuf_core_$(TARGET)

#------------------------------------------------------------------------------
#  JAVA SETUP
#------------------------------------------------------------------------------
JAVA            = java
JFLAGS          = -jar
JPROTO_FLAGS    = --c_out=$(GEN_SOURCES_DIR) #-I=$(PROTO_FILES_DIR)
PROTOBUF_DIR    = protobuf_shared
JAR_FILE        = $(PROTOBUF_DIR)/protoc-1.0M2.jar

#------------------------------------------------------------------------------
#  PYTHON SETUP
#------------------------------------------------------------------------------
PYTHON          = python
PFLAGS          = -i $(GEN_SOURCES_DIR)/rfid_board_api.c
LIB_SPLITTER    = lib_splitter_shared/src/protobuf_lib_splitter.py

#------------------------------------------------------------------------------
# Build rules
#------------------------------------------------------------------------------
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


all: $(BIN_DIR) $(BIN_DIR)/rfid_board_api.o $(BIN_DIR)/protobuf_core.o $(LIBDIR)/$(LIB_NAME).a $(LIBDIR)/$(LIB_CORE).a
	$(RANLIB) $(LIBDIR)/$(LIB_CORE).a
	$(RANLIB) $(LIBDIR)/$(LIB_NAME).a

$(GEN_SOURCES_DIR)/rfid_board_api.c: $(PROTO_FILES_DIR)/rfid_board_api.proto 
	@echo --- Regenerating code from proto files ---
	$(TRACE_FLAG)$(MKDIR) $(GEN_SOURCES_DIR)
	$(TRACE_FLAG)$(JAVA) $(JFLAGS) $(JAR_FILE) $(JPROTO_FLAGS) -I=$(PROTO_FILES_DIR) $(PROTO_FILES)
	@echo --- Splitting out core functionality ---
	$(TRACE_FLAG)$(PYTHON) $(LIB_SPLITTER) $(PFLAGS)
	
$(LIBDIR)/$(LIB_NAME).a:  $(C_OBJS_EXT) $(LIBDIR)
	@echo --- Compiling Protocol library ---
	$(TRACE_FLAG)$(AR) $(LIBFLAGS) $(LIBDIR)/$(LIB_NAME).a $(C_OBJS_EXT)

$(LIBDIR)/$(LIB_CORE).a:  $(C_CORE_OBJS_EXT) $(LIBDIR)
	@echo --- Compiling Core library ---
	$(TRACE_FLAG)$(AR) $(LIBFLAGS) $(LIBDIR)/$(LIB_CORE).a $(C_CORE_OBJS_EXT)
	
$(BIN_DIR)/rfid_board_api.o : $(GEN_SOURCES_DIR)/rfid_board_api.c 
	@echo --- Creating Object files ---
	$(TRACE_FLAG)$(CC) $(CFLAGS) -c $< -o $@
	
$(BIN_DIR)/protobuf_core.o : $(GEN_SOURCES_DIR)/protobuf_core.c 
	@echo --- Creating Object files ---
	$(TRACE_FLAG)$(CC) $(CFLAGS) -c $< -o $@
	
#$(BIN_DIR)/dfuse.o : $(SOURCES_DIR)/dfuse.c 
#	@echo --- Creating Object files ---
#	$(TRACE_FLAG)$(CC) $(CFLAGS) -c $< -o $@

$(BIN_DIR)/%.o : $(SOURCES_DIR)/%.c 
	@echo --- Creating Object files ---
	$(TRACE_FLAG)$(CC) $(CFLAGS) -c $< -o $@

$(BIN_DIR) $(GEN_SOURCES_DIR) $(LIBDIR):
	@echo --- Creating directories ---
	$(TRACE_FLAG)$(MKDIR) $@

clean:
	@echo --- Cleaning obj and dep files ONLY ---
	$(TRACE_FLAG)$(RM) *.o *.d $(BIN_DIR)/*.o $(BIN_DIR)/*.d
    
cleanlibs: clean
	@echo --- Cleaning ALL generated libraries ---
	$(TRACE_FLAG)$(RM) *.a arm_lib/*.a ia32_lib/*.a

cleandirs: cleanlibs
	@echo --- Cleaning generated directories ---
	$(TRACE_FLAG)$(RM) $(GEN_SOURCES_DIR) ppc_lib arm_lib ia32_lib $(BIN_DIR)
	
cleanall: cleandirs
	@echo --- Cleaning generated *.h and *.c files ---
	$(TRACE_FLAG)$(RM) *.c *.h  $(GEN_SOURCES_DIR)/*.c  $(GEN_SOURCES_DIR)/*.h
	
show:
	@echo --- Printing Variables ---
	@echo PROTO_FILES = $(PROTO_FILES)
	@echo C_SRCS = $(C_SRCS)
	@echo C_OBJS_EXT = $(C_OBJS_EXT)
	@echo C_DEPS_EXT = $(C_DEPS_EXT)
	@echo LIBS = $(LIBS)
	@echo CFLAGS = $(CFLAGS)
