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
TRACE                   = 0
TRACEON                 = $(TRACE:0=@)
TRACE_FLAG              = $(TRACEON:1=)

# env.mk contains an optional CONF define (as above) and IP define
# if IP=slave the default IP address built into the code will be 169.254.2.3
# if not, then the user's printer IP will be built in.
ifeq ($(IP),)
  -include env.mk
endif

# config.mk also must contains the version number components
# config.mk and env.mk must be separate since env.mk is NOT version 
# controlled; config.mk is.
-include config.mk

# If DCC_IP is defined, but not the MAC, complain
ifneq ($(IP),)
  ifeq ($(MAC),)
    $(error Please include MAC with IP! );
  endif
endif

ifeq ($(IP),)       # Slave IP address............................
IPADDR0                 = 172  
IPADDR1                 = 27
IPADDR2                 = 0
IPADDR3                 = 3
MAC                     = 0x3b
else
IPADDR0                 = 172
IPADDR1                 = 27
IPADDR2                 = 0
IPADDR3                 = $(IP)
endif

NOW                    := $(shell date +%Y%m%d%H%M%S | tr ' :' '__')

# Output file basename (should be the same as the directory name)
PROJECT_NAME            = DC3$(FW)

# This define specifies that the code that's being built is the application code
# as opposed to the bootloader code.  This is used by some shared code to leave/
# include some functionality. 
DEFINES                 = -DCPLR_APP

#------------------------------------------------------------------------------
#  MCU SETUP - This specifies which core to pass down to all the other makefiles
#  and to compile options for different ARM cortex-m processors.
#------------------------------------------------------------------------------
MCU                     = cortex-m4-fpv4-sp-d16

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

#-------------------------------------------------------------------------------
#  TOOLCHAIN SETUP
#-------------------------------------------------------------------------------
CROSS                   = arm-none-eabi-
CC                      = $(CROSS)gcc
CPP                     = $(CROSS)g++
AS                      = $(CROSS)as
LINK                    = $(CROSS)gcc
OBJCPY                  = $(CROSS)objcopy
SIZE                    = $(CROSS)size
RM                      = rm -rf
ECHO                    = echo
MKDIR                   = mkdir

#-------------------------------------------------------------------------------
# General Directories
#-------------------------------------------------------------------------------
SRC_DIR                 = ./src
COMMON_FW_SRC_DIR       = ../Common
COMMON_CLI_SRC_DIR      = ../../Common

# Local app, bsp, and sys directories
APP_DIR                 = $(SRC_DIR)/app
BSP_DIR                 = $(SRC_DIR)/bsp
SYS_DIR                 = $(SRC_DIR)/sys

# app, bsp, and sys directories common between Bootloader and Application
COMMON_FW_BSP_DIR       = $(COMMON_FW_SRC_DIR)/bsp
COMMON_FW_SYS_DIR       = $(COMMON_FW_SRC_DIR)/sys

# app, bsp, and sys directories common between firmware and client
COMMON_CLI_BSP_DIR       = $(COMMON_CLI_SRC_DIR)/bsp
COMMON_CLI_SYS_DIR       = $(COMMON_CLI_SRC_DIR)/sys

#-------------------------------------------------------------------------------
# BSP and Driver Directories
#-------------------------------------------------------------------------------
# Ethernet Driver
ETH_DRV_DIR             = $(COMMON_FW_BSP_DIR)/STM32F4x7_ETH_Driver
QP_LWIP_PORT_DIR        = $(COMMON_FW_BSP_DIR)/qpc_lwip_port

# Serial Driver and AO
SERIAL_DIR              = $(COMMON_FW_BSP_DIR)/serial

# I2C Driver and AO
I2C_DIR                 = $(COMMON_FW_BSP_DIR)/i2c
I2C_FRT_DIR             = $(BSP_DIR)/i2c

# NOR Driver and AO
NOR_DIR                 = $(BSP_DIR)/nor

# SDRAM Driver and AO
SDRAM_DIR               = $(COMMON_FW_BSP_DIR)/sdram

# STM32 Drivers
STM32F4XX_STD_PERIPH_DIR= $(COMMON_FW_BSP_DIR)/STM32F4xx_StdPeriph_Driver

# Directories that need to be passed down to LWIP.  Don't use $(VAR) here since
# these get passed down to LWIP as a string and $(VAR)s won't be evaluated.
LWIP_PORT_FOR_LWIP      = ../../bsp/qpc_lwip_port
BSP_DIR_FOR_LWIP        = ../../bsp/runtime
LWIP_SRC_DIR            = ../../../Application/src

#-------------------------------------------------------------------------------
# SYS and RTOS Directories
#-------------------------------------------------------------------------------
# QPC directories
QPC_DIR                 = $(COMMON_FW_SYS_DIR)/qpc_5.3.1
QP_PORT_DIR             = $(QPC_DIR)/ports/freertos/gnu
QP_FR_CONF_DIR          = $(QPC_DIR)/ports/freertos/conf

# FreeRTOS directories
FR_DIR                  = $(SYS_DIR)/FreeRTOSV8.1.2/FreeRTOS/Source
FR_PORT_DIR             = $(FR_DIR)/portable/GCC/ARM_CM4F
FR_INC_DIR              = $(FR_DIR)/include

# LWIP
LWIP_DIR                = $(COMMON_FW_SYS_DIR)/lwip

# Base64 encoding module
BASE64_DIR              = $(COMMON_CLI_SYS_DIR)/libb64

# K-ary tree directory
KTREE_DIR               = $(SYS_DIR)/ktree

# Coupler Board API directory
DC3_API_DIR                  = $(COMMON_CLI_SYS_DIR)/DC3_api
DC3_API_GEN_SRC_DIR          = $(DC3_API_DIR)/gen_src
DC3_API_SRC_DIR              = $(DC3_API_DIR)/src
DC3_API_INC_DIR              = $(DC3_API_DIR)/inc
DC3_API_LIB_DIR              = $(DC3_API_DIR)/arm_lib

#-------------------------------------------------------------------------------
# APP Directories
#-------------------------------------------------------------------------------
# Application Comm directory
APP_COMM_DIR            = $(APP_DIR)/comm

# Application Coupler directory
APP_CPLR_DIR            = $(APP_DIR)/cplr

# Application Debug directory
APP_DBG_DIR             = $(APP_DIR)/debug

# Application Menu and submenu directories
APP_MENU_DIR                      = $(APP_DIR)/menu
APP_MENU_DBG_DIR                  = $(APP_MENU_DIR)/dbgMenu
APP_MENU_DBG_OUT_CNTRL_DIR        = $(APP_MENU_DBG_DIR)/dbgOutCntrlMenu
APP_MENU_DBG_MOD_CNTRL_DIR        = $(APP_MENU_DBG_DIR)/dbgModCntrlMenu
APP_MENU_SYSTEST_DIR              = $(APP_MENU_DIR)/sysTestMenu
APP_MENU_SYSTEST_I2C              = $(APP_MENU_SYSTEST_DIR)/sysI2CTests

# Collect all the menu directories and subdirectories here so we don't have to
# keep modifying the VPATH and INCLUDE paths.
APP_MENU_DIRS           = $(APP_MENU_DIR) \
                          $(APP_MENU_DBG_DIR) \
                          $(APP_MENU_DBG_OUT_CNTRL_DIR) \
                          $(APP_MENU_DBG_MOD_CNTRL_DIR) \
                          $(APP_MENU_SYSTEST_DIR) \
                          $(APP_MENU_SYSTEST_I2C)
                          
APP_MENU_INCLUDES       = -I$(APP_MENU_DIR) \
                          -I$(APP_MENU_DBG_DIR) \
                          -I$(APP_MENU_DBG_OUT_CNTRL_DIR) \
                          -I$(APP_MENU_DBG_MOD_CNTRL_DIR) \
                          -I$(APP_MENU_SYSTEST_DIR) \
                          -I$(APP_MENU_SYSTEST_I2C)
                                                 
# Collect all the menu c source files here so we don't have to the list of sources
MENU_CSRCS              = ktree.c \
                          menu.c \
                          menu_top.c \
                          debug_menu.c \
                          dbg_out_cntrl.c \
                          dbg_mod_cntrl.c \
                          systest_menu.c \
                          systest_i2c.c						  

#-------------------------------------------------------------------------------
# Source virtual directories - this is where make will look for all files that
# it needs to compile.
#-------------------------------------------------------------------------------
VPATH                   = $(APP_DIR) \
                          $(APP_CPLR_DIR) \
                          \
                          $(BSP_DIR) \
                          $(COMMON_FW_BSP_DIR) \
                          $(ETH_DRV_DIR)/src \
                          $(SERIAL_DIR) \
                          $(I2C_DIR) \
                          $(I2C_FRT_DIR) \
                          $(NOR_DIR) \
                          $(SDRAM_DIR) \
                          \
                          $(QP_LWIP_PORT_DIR) \
                          $(QP_LWIP_PORT_DIR)/netif \
                          \
                          $(BASE64_DIR) \
                          $(COMMON_FW_BSP_DIR)/runtime \
                          \
                          $(STM32F4XX_STD_PERIPH_DIR)/src \
                          \
                          $(COMMON_FW_SYS_DIR)

#-------------------------------------------------------------------------------
# Include directories - this is where make will look for all header files that
# the source files need to compile.
#-------------------------------------------------------------------------------
INCLUDES                = -I$(SRC_DIR) \
                          -I$(COMMON_FW_SRC_DIR) \
                          -I$(COMMON_CLI_SYS_DIR) \
                          -I$(APP_DIR) \
                          \
                          -I$(APP_COMM_DIR) \
                          -I$(APP_CPLR_DIR) \
                          \
                          -I$(QPC_DIR)/include \
                          -I$(QPC_DIR)/qf/source \
                          -I$(QP_PORT_DIR) \
                          \
                          -I$(BSP_DIR)  \
                          -I$(COMMON_FW_BSP_DIR) \
                          -I$(ETH_DRV_DIR)/inc \
                          -I$(BASE64_DIR) \
                          -I$(COMMON_FW_BSP_DIR)/runtime \
                          -I$(SERIAL_DIR) \
                          -I$(I2C_DIR) \
                          -I$(I2C_FRT_DIR) \
                          -I$(NOR_DIR) \
                          -I$(SDRAM_DIR) \
                          \
                          -I$(LWIP_DIR)/src/include \
                          -I$(LWIP_DIR)/src/include/netif \
                          -I$(LWIP_DIR)/src/include/lwip \
                          -I$(LWIP_DIR)/src/include/ipv4 \
                          \
                          -I$(COMMON_FW_BSP_DIR)/CMSIS/Include \
                          -I$(COMMON_FW_BSP_DIR)/CMSIS/Device/ST/STM32F4xx/Include \
                          \
                          -I$(QP_LWIP_PORT_DIR) \
                          -I$(QP_LWIP_PORT_DIR)/arch \
                          -I$(QP_LWIP_PORT_DIR)/netif \
                          \
                          -I$(STM32F4XX_STD_PERIPH_DIR)/inc \
                          \
                          -I$(COMMON_FW_SYS_DIR) \
                          \
                          -I$(FR_INC_DIR) \
                          -I$(QP_FR_CONF_DIR) \
                          -I$(FR_PORT_DIR) \
                          \
                          -I$(DC3_API_SRC_DIR) \
                          -I$(DC3_API_INC_DIR) \
                          -I$(DC3_API_GEN_SRC_DIR)

#-------------------------------------------------------------------------------
# defines
# HSE_VALUE=8000000 overrides the External Clock setting for the 
# board since the development kit runs at a 25MHz external clock
#-------------------------------------------------------------------------------
#DEFINES   			   += -DSTM32F439xx \
#						  -DUSE_STDPERIPH_DRIVER \
#						  -DHSE_VALUE=8000000 \
#						  -DLWIP_TCP=1 \
#						  -DFLASH_BASE=0x08000000

# Temporary for devel kit						  
DEFINES                += -DSTM32F429_439xx \
                          -DUSE_STDPERIPH_DRIVER \
                          -DLWIP_TCP=1 \
                          -DFLASH_BASE=0x08040000
						  
#-------------------------------------------------------------------------------
# All source files that need to be compiled
#-------------------------------------------------------------------------------

# assembler source files
ASM_SRCS               := \
                          startup_stm32f429xx.S \
                          memcpy.S \
                          stm32_hardfault_handler.S

# C source files
C_SRCS                  = \
                          main.c \
                          no_heap.c \
                          cplr.c \
                          \
                          bsp.c \
                          system_stm32f4xx.c \
                          stm32f4xx_it.c \
                          \
                          stm32f4x7_eth.c \
                          stm32f4x7_eth_bsp.c \
                          eth_driver.c \
                          lwip.c \
                          \
                          syscalls.c \
                          \
                          serial.c \
                          console_output.c \
                          time.c \
                          i2c.c \
                          i2c_dev.c \
                          i2c_frt.c \
                          nor.c \
                          sdram.c \
                          dbg_cntrl.c \
                          db.c \
                          flash.c \
                          \
                          cencode.c \
                          cdecode.c \
                          base64_wrapper.c \
                          \
                          LWIPMgr.c \
                          I2CBusMgr.c \
                          I2C1DevMgr.c \
                          SerialMgr.c \
                          CommMgr.c \
                          SysMgr.c \
                          \
                          misc.c  \
                          stm32f4xx_crc.c \
                          stm32f4xx_dma.c \
                          stm32f4xx_exti.c \
                          stm32f4xx_fmc.c \
                          stm32f4xx_i2c.c \
                          stm32f4xx_gpio.c \
                          stm32f4xx_pwr.c \
                          stm32f4xx_rcc.c \
                          stm32f4xx_rtc.c \
                          stm32f4xx_syscfg.c \
                          stm32f4xx_tim.c \
                          stm32f4xx_usart.c
			      		
			      		
# Temporarily not compiled files.  These will need to be added back in before
# everything is done but for now, leave out. HR			      		
#			      		flash_if.c \
			      		
# Add these back in once that hardware is actually needed. HR.			      		
#			      		stm32f4xx_adc.c  \
#			      		stm32f4xx_can.c \
#			      		stm32f4xx_cryp.c \
#			      		stm32f4xx_cryp_aes.c \
#			      		stm32f4xx_cryp_des.c \
#			      		stm32f4xx_cryp_tdes.c \
#			      		stm32f4xx_dac.c \
#			      		stm32f4xx_dbgmcu.c \
#			      		stm32f4xx_dcmi.c \
#			      		stm32f4xx_dma2d.c \
#                       stm32f4xx_flash.c \
#			      		stm32f4xx_hash.c \
#			      		stm32f4xx_hash_md5.c \
#			      		stm32f4xx_hash_sha1.c \
#			      		stm32f4xx_iwdg.c \
#			      		stm32f4xx_ltdc.c \
#			      		stm32f4xx_rng.c \
#			      		stm32f4xx_sai.c \
#			      		stm32f4xx_sdio.c \
#			      		stm32f4xx_spi.c \
#			      		stm32f4xx_wwdg.c
						
# C++ source files
CPP_SRCS 				:=	$(wildcard *.cpp)


LD_SCRIPT 				:= $(BSP_DIR)/linkerscript/stm32f439flash.ld
LDFLAGS  				:= -T$(LDSCRIPT)

#-------------------------------------------------------------------------------
# build options for various configurations
#-------------------------------------------------------------------------------

# Common options for all configurations. 
LIBS    	= -lqp_$(ARM_CORE)_cs -llwip_$(ARM_CORE)_cs -lDC3CommApi_arm -lprotobuf_core_arm
LIB_PATHS   = -L$(QP_PORT_DIR)/$(BIN_DIR) -L$(LWIP_DIR)/$(BIN_DIR) -L$(DC3_API_LIB_DIR)

# Specific options depending on the build configuration
ifeq (rel, $(CONF))       # Release configuration ............................

BIN_DIR 	:= rel
DEFINES		+= -DNDEBUG
ASFLAGS     = $(MFLAGS)
CFLAGS      = $(MFLAGS) -std=gnu99 -Wall -ffunction-sections -fdata-sections \
			  -Os $(INCLUDES) $(DEFINES)

CPPFLAGS    = $(MFLAGS) \
			  -Wall -fno-rtti -fno-exceptions \
			  -Os $(INCLUDES) $(DEFINES)

LINKFLAGS   = -nodefaultlibs -Xlinker --gc-sections -Wl,--strip-all \
              -Wl,-Map,$(BIN_DIR)/$(PROJECT_NAME).map $(MFLAGS)

else ifeq (spy, $(CONF))  # Spy configuration ................................

BIN_DIR 	:= spy
DEFINES		+= -DQ_SPY
ASFLAGS 	= $(MFLAGS)
CFLAGS 		= $(MFLAGS) -std=gnu99 -Wall -ffunction-sections -fdata-sections \
			  -Os -O $(INCLUDES) $(DEFINES)

CPPFLAGS 	= $(MFLAGS) \
			  -Wall -fno-rtti -fno-exceptions \
			  -O $(INCLUDES) $(DEFINES)

LINKFLAGS 	= -nodefaultlibs -Xlinker --gc-sections -Wl,--strip-all \
              -Wl,-Map,$(BIN_DIR)/$(PROJECT_NAME).map $(MFLAGS)

else                     # default Debug configuration .......................

BIN_DIR 	:= dbg
ASFLAGS 	= -g3 -ggdb $(MFLAGS)
CFLAGS 		= $(MFLAGS) -std=gnu99 -Wall -ffunction-sections -fdata-sections \
              -Wl,--gc-sections \
			  -g -ggdb -O0 $(INCLUDES) $(DEFINES)

CPPFLAGS 	= $(MFLAGS) \
			  -Wall -fno-rtti -fno-exceptions \
			  -g3 -ggdb -O0 $(INCLUDES) $(DEFINES)
	
LINKFLAGS 	= -nodefaultlibs -Xlinker --gc-sections \
              -Wl,-Map,$(BIN_DIR)/$(PROJECT_NAME).map $(MFLAGS)

endif

ASM_OBJS     = $(patsubst %.S,%.o,$(ASM_SRCS))
C_OBJS       = $(patsubst %.c,%.o,$(C_SRCS))
CPP_OBJS     = $(patsubst %.cpp,%.o,$(CPP_SRCS))

TARGET_BIN   = $(BIN_DIR)/$(PROJECT_NAME).hex
TARGET_ELF   = $(BIN_DIR)/$(PROJECT_NAME).elf
TARGET_FLSH  = $(BIN_DIR)/$(PROJECT_NAME)_v$(MAJOR).$(MINOR)_$(NOW).bin

ASM_OBJS_EXT = $(addprefix $(BIN_DIR)/, $(ASM_OBJS))
C_OBJS_EXT   = $(addprefix $(BIN_DIR)/, $(C_OBJS))
C_DEPS_EXT   = $(patsubst %.o, %.d, $(C_OBJS_EXT))
CPP_OBJS_EXT = $(addprefix $(BIN_DIR)/, $(CPP_OBJS))
CPP_DEPS_EXT = $(patsubst %.o, %.d, $(CPP_OBJS_EXT))

REAL_VER     = APP_$(BOARD)$(SERIES)_$(PRODUCT)_$(MAINT)_$(DEVIATION)
SPECIAL_VER  = APP_$(BOARD)$(SERIES)_$(PRODUCT)_$(MAINT)_$(SPECIAL)
DEBUG_VER    = APP_$(BOARD)$(SERIES)_0_$(MAINT)_65535

#-----------------------------------------------------------------------------
# rules
#

# Default rule:
all: ver build_IP $(BIN_DIR) build_libs $(TARGET_BIN) showver

showver:
	@if [ $(IPADDR0) == 169 ] ; then \
	   echo;\
	   echo -------------------------------------------------;\
		cat $(SRC_DIR)/version.h | grep FW_VER | cut -b 9-;\
	   echo         Built for IP address $(IPADDR0).$(IPADDR1).$(IPADDR2).$(IPADDR3)    --SLAVE DEVICE--;\
	   echo -------------------------------------------------;\
	   echo;\
	else\
	   echo;\
	   echo +++++++++++++++++++++++++++++++++;\
		cat $(SRC_DIR)/version.h | grep FW_VER | cut -b 9-;\
	   echo         Built for IP address $(IPADDR0).$(IPADDR1).$(IPADDR2).$(IPADDR3);\
	   echo +++++++++++++++++++++++++++++++++;\
	   echo;\
	fi

ver: config.mk
	@echo "#define "FW_VER_MAJOR" $(MAJOR)" > $(SRC_DIR)/version.h;
	@echo "#define "FW_VER_MINOR" $(MINOR)" >> $(SRC_DIR)/version.h;
	@if [ $(MAJOR) -gt 9 ] && [ $(MINOR) -gt 9 ] ; then \
		echo "#define "FW_VER" \"v$(MAJOR).$(MINOR)\"" >> $(SRC_DIR)/version.h; \
	elif [ $(MAJOR) -lt 10 ] && [ $(MINOR) -gt 9 ] ; then \
		echo "#define "FW_VER" \"v0$(MAJOR).$(MINOR)\"" >> $(SRC_DIR)/version.h; \
	elif [ $(MAJOR) -lt 10 ] && [ $(MINOR) -lt 10 ] ; then \
		echo "#define "FW_VER" \"v0$(MAJOR).0$(MINOR)\"" >> $(SRC_DIR)/version.h; \
	elif [ $(MAJOR) -gt 9 ] && [ $(MINOR) -lt 10 ] ; then \
		echo "#define "FW_VER" \"v$(MAJOR).0$(MINOR)\"" >> $(SRC_DIR)/version.h; \
	fi
	@echo "#define "BUILD_DATE" \"$(NOW)\"" >> $(SRC_DIR)/version.h

# All the rigamarole below is to avoid building code if the ip address didn't change (normal situation for development)
build_IP:
	@if [ ! -e $(SRC_DIR)/ipAndMac.h ]; then echo "placeholder" > $(SRC_DIR)/ipAndMac.h; fi
	@echo "#define STATIC_IPADDR0" "$(IPADDR0)" >> $(SRC_DIR)/ipAndMac.new.h
	@echo "#define STATIC_IPADDR1" "$(IPADDR1)" >> $(SRC_DIR)/ipAndMac.new.h
	@echo "#define STATIC_IPADDR2" "$(IPADDR2)" >> $(SRC_DIR)/ipAndMac.new.h
	@echo "#define STATIC_IPADDR3" "$(IPADDR3)" >> $(SRC_DIR)/ipAndMac.new.h
	@echo "#define DCC_MAC" $(MAC)"" >> $(SRC_DIR)/ipAndMac.new.h
	@-diff $(SRC_DIR)/ipAndMac.new.h $(SRC_DIR)/ipAndMac.h > $(SRC_DIR)/ipAndMac.diff
	@if [ -s $(SRC_DIR)/ipAndMac.diff ]; then mv $(SRC_DIR)/ipAndMac.new.h $(SRC_DIR)/ipAndMac.h; fi
	@-rm -f $(SRC_DIR)/ipAndMac.diff $(SRC_DIR)/ipAndMac.new.h

$(BIN_DIR):
	@echo --- Creating directory $@
	$(TRACE_FLAG)mkdir -p $@

$(TARGET_BIN): $(TARGET_ELF) cleanbin
	@echo --- Creating hex $@
	$(TRACE_FLAG)$(OBJCPY) -O binary $< $@
	$(TRACE_FLAG)mv $(TARGET_BIN) $(TARGET_FLSH)
	@echo --- Creating bin $(TARGET_FLSH)
	$(TRACE_FLAG)$(OBJCPY) -O ihex $< $@
	
$(TARGET_ELF) : $(ASM_OBJS_EXT) $(C_OBJS_EXT) $(CPP_OBJS_EXT)
	@echo --- Linking libraries
	$(TRACE_FLAG)$(LINK) -T$(LD_SCRIPT) $(LINKFLAGS) $(LIB_PATHS) -o $@ $^ $(LIBS)
	$(SIZE) $(TARGET_ELF)

build_libs: build_qpc build_lwip build_DC3_api
	@echo -----------------------------------
	@echo --- Building libraries complete ---
	@echo -----------------------------------

build_qpc:
	@echo ------------------------------------------------
	@echo --- Building QPC libraries in $(QP_PORT_DIR) ---
	@echo ------------------------------------------------
	$(TRACE_FLAG)cd $(QP_PORT_DIR); make TRACE=$(TRACE) MCU=$(MCU) CONF=$(BIN_DIR)

build_DC3_api:
	@echo ------------------------------------------------
	@echo --- Building DC3 API library in $(DC3_API_DIR) ---
	@echo ------------------------------------------------
	cd $(DC3_API_DIR); make TARGET=arm  MCU=$(MCU) TRACE=$(TRACE) clean
	$(TRACE_FLAG)cd $(DC3_API_DIR); make TARGET=arm  MCU=$(MCU) TRACE=$(TRACE) all

build_lwip:
	@echo ------------------------------------------------
	@echo --- Building LWIP library in $(LWIP_DIR)     ---
	@echo ------------------------------------------------
	$(TRACE_FLAG)cd $(LWIP_DIR); make MCU=$(MCU) CONF=$(BIN_DIR) LWIP_PORT_DIR=$(LWIP_PORT_FOR_LWIP) PROJ_BSP=$(BSP_DIR_FOR_LWIP) LWIP_SRC_DIR=$(LWIP_SRC_DIR) TRACE=$(TRACE)

$(BIN_DIR)/%.d : %.c
	@echo --- Checking dependency for $(<F)
	$(TRACE_FLAG)$(CC) -MM -MT $(@:.d=.o) $(CFLAGS) $< > $@

$(BIN_DIR)/%.d : %.cpp
	@echo --- Compiling $(<F)
	$(TRACE_FLAG)$(CPP) -MM -MT $(@:.d=.o) $(CPPFLAGS) $< > $@

$(BIN_DIR)/%.o : %.S
	@echo --- Compiling $(<F)
	$(TRACE_FLAG)$(AS) $(ASFLAGS) $< -o $@

$(BIN_DIR)/%.o : %.c
	@echo --- Compiling $(<F)
	$(TRACE_FLAG)$(CC) $(CFLAGS) -c $< -o $@

$(BIN_DIR)/%.o : %.cpp
	@echo --- Compiling $(<F)
	$(TRACE_FLAG)$(CPP) $(CPPFLAGS) -c $< -o $@

# Make sure not to generate dependencies when doing cleans
NODEPS:=clean cleanall cleanlibs cleandirs
ifeq (0, $(words $(findstring $(MAKECMDGOALS), $(NODEPS))))
-include $(C_DEPS_EXT) $(CPP_DEPS_EXT)
endif

.PHONY : clean clean_with_libs cleanbin
cleanall:
	@echo ---------------------------
	@echo --- Cleaning EVERYTHING
	@echo ---------------------------
	$(TRACE_FLAG)cd $(QP_PORT_DIR); make MCU=$(MCU) CONF=dbg clean
	$(TRACE_FLAG)cd $(QP_PORT_DIR); make MCU=$(MCU) CONF=spy clean
	$(TRACE_FLAG)cd $(QP_PORT_DIR); make MCU=$(MCU) CONF=rel clean
	$(TRACE_FLAG)cd $(LWIP_DIR); make LWIP_PORT_DIR=$(LWIP_PORT_FOR_LWIP) PROJ_BSP=$(BSP_DIR_FOR_LWIP) LWIP_SRC_DIR=$(LWIP_SRC_DIR) MCU=$(MCU) CONF=dbg clean
	$(TRACE_FLAG)cd $(LWIP_DIR); make LWIP_PORT_DIR=$(LWIP_PORT_FOR_LWIP) PROJ_BSP=$(BSP_DIR_FOR_LWIP) LWIP_SRC_DIR=$(LWIP_SRC_DIR) MCU=$(MCU) CONF=spy clean
	$(TRACE_FLAG)cd $(LWIP_DIR); make LWIP_PORT_DIR=$(LWIP_PORT_FOR_LWIP) PROJ_BSP=$(BSP_DIR_FOR_LWIP) LWIP_SRC_DIR=$(LWIP_SRC_DIR) MCU=$(MCU) CONF=rel clean
	-$(RM) dbg/*.o dbg/*.d dbg/*.hex dbg/*.elf dbg/*.map dbg
	-$(RM) spy/*.o spy/*.d spy/*.hex spy/*.elf spy/*.map rel
	-$(RM) rel/*.o rel/*.d rel/*.hex rel/*.elf rel/*.map spy
	
clean:
	$(TRACE_FLAG)-$(RM) $(BIN_DIR)/*.o \
	$(TRACE_FLAG)$(BIN_DIR)/*.d \
	$(TRACE_FLAG)$(BIN_DIR)/*.hex \
	$(TRACE_FLAG)$(BIN_DIR)/*.elf \
	$(TRACE_FLAG)$(BIN_DIR)/*.map

cleanbin:
	@echo --- Cleaning only *.bin files 
	$(TRACE_FLAG)-$(RM) $(BIN_DIR)/*.bin
	
clean_with_libs: clean clean_qpc_libs
	@echo ---------------------------
	@echo --- Cleaned All libs and bins
	@echo ---------------------------
	
clean_qpc_libs:
	@echo ---------------------------
	@echo --- Cleaning QPC libraries
	@echo ---------------------------
	$(TRACE_FLAG)cd $(QP_PORT_DIR); make MCU=$(MCU) CONF=$(BIN_DIR) clean

show:
	@echo CONF = $(CONF)
	@echo ASM_SRCS = $(ASM_SRCS)
	@echo C_SRCS = $(C_SRCS)
	@echo CPP_SRCS = $(CPP_SRCS)
	@echo ASM_OBJS_EXT = $(ASM_OBJS_EXT)
	@echo C_OBJS_EXT = $(C_OBJS_EXT)
	@echo C_DEPS_EXT = $(C_DEPS_EXT)
	@echo CPP_DEPS_EXT = $(CPP_DEPS_EXT)
	@echo TARGET_ELF = $(TARGET_ELF)
	@echo LIBS = $(LIBS)
	@echo LIB_PATHS = $(LIB_PATHS)
	@echo MCU = $(MCU)
	@echo MFLAGS = $(MFLAGS)
	@echo CFLAGS = $(CFLAGS)
	@echo COMMON_FW_BSP_DIR = $(COMMON_FW_BSP_DIR)
	@echo COMMON_FW_SYS_DIR = $(COMMON_FW_SYS_DIR)
	@echo I2C_FRT_DIR = $(I2C_FRT_DIR)
	@echo BSP_DIR = $(BSP_DIR)
	@echo NOW = $(NOW)