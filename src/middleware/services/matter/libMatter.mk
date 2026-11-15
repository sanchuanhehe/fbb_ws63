#!/bin/bash
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
# This file is used to generate CHIP core library
#
CROSS_COMPILE = $(TOOLCHAIN_PATH)/bin/$(TOOLCHAIN_PREFIX)
CHIP_DIR = ${BASEDIR}/middleware/services/matter/connectedhomeip

# Compilation tools
AR = $(CROSS_COMPILE)ar
CXX = $(CROSS_COMPILE)g++
CC = $(CROSS_COMPILE)gcc
AS = $(CROSS_COMPILE)as
NM = $(CROSS_COMPILE)nm
LD = $(CROSS_COMPILE)gcc
GDB = $(CROSS_COMPILE)gdb
OBJCOPY = $(CROSS_COMPILE)objcopy
OBJDUMP = $(CROSS_COMPILE)objdump

Q := @
ifeq ($(V),1)
Q := 
endif
OS := $(shell uname)

ifeq ($(ECHO),)
ECHO=echo
endif

# -------------------------------------------------------------------
# Include folder list
# -------------------------------------------------------------------
INCLUDES =
INCLUDES += -I$(BASEDIR)/include
INCLUDES += -I$(BASEDIR)/include/middleware/services/wifi
INCLUDES += -I$(BASEDIR)/include/middleware/services/bts/common
INCLUDES += -I$(BASEDIR)/include/middleware/services/bts/ble
INCLUDES += -I$(BASEDIR)/include/middleware/utils
INCLUDES += -I$(BASEDIR)/middleware/chips/ws63/dfx/include
INCLUDES += -I$(BASEDIR)/middleware/chips/ws63/update/include
INCLUDES += -I$(BASEDIR)/middleware/chips/ws63/partition/include
INCLUDES += -I$(BASEDIR)/middleware/chips/ws63/nv/include
INCLUDES += -I$(BASEDIR)/middleware/chips/ws63/nv/nv_config/include
INCLUDES += -I$(BASEDIR)/middleware/utils/common_headers

INCLUDES += -I$(BASEDIR)/kernel/liteos/liteos_v208.5.0/Huawei_LiteOS/kernel/include
INCLUDES += -I$(BASEDIR)/kernel/liteos/liteos_v208.5.0/Huawei_LiteOS/targets/ws63/include
INCLUDES += -I$(BASEDIR)/kernel/liteos/liteos_v208.5.0/Huawei_LiteOS/arch/riscv/include
INCLUDES += -I$(BASEDIR)/kernel/liteos/liteos_v208.5.0/Huawei_LiteOS/drivers/timer/include
INCLUDES += -I$(BASEDIR)/kernel/liteos/liteos_v208.5.0/Huawei_LiteOS/platform/libsec/include
INCLUDES += -I$(BASEDIR)/kernel/liteos/liteos_v208.5.0/Huawei_LiteOS/drivers/interrupt/include
INCLUDES += -I$(BASEDIR)/kernel/liteos/liteos_v208.5.0/Huawei_LiteOS/open_source/musl/include
INCLUDES += -I$(BASEDIR)/kernel/liteos/liteos_v208.5.0/Huawei_LiteOS/lib/liteos_libc

INCLUDES += -I$(BASEDIR)/kernel/liteos/liteos_v208.5.0/Huawei_LiteOS/open_source/CMSIS/CMSIS/RTOS2/Include
INCLUDES += -I$(BASEDIR)/kernel/liteos/liteos_v208.5.0/Huawei_LiteOS/platform/libsec/include
INCLUDES += -I$(BASEDIR)/drivers/chips/ws63/include
INCLUDES += -I$(BASEDIR)/drivers/chips/ws63/porting/pinctrl
INCLUDES += -I$(BASEDIR)/drivers/drivers/hal/gpio
INCLUDES += -I$(BASEDIR)/drivers/chips/ws63/porting/gpio
INCLUDES += -I$(BASEDIR)/drivers/drivers/hal/gpio/v150
INCLUDES += -I$(BASEDIR)/drivers/drivers/hal/reboot


INCLUDES += -I$(BASEDIR)/open_source/mbedtls/mbedtls_v3.6.0/include
INCLUDES += -I$(BASEDIR)/open_source/mbedtls/mbedtls_v3.6.0/include/psa

INCLUDES += -I$(BASEDIR)/drivers/drivers/driver/security_unified/mbedtls_harden_adapt
INCLUDES += -I$(BASEDIR)/drivers/drivers/driver/security_unified/mbedtls_harden_adapt/include

INCLUDES += -I$(BASEDIR)/open_source/lwip/lwip_v2.1.3/src/include
INCLUDES += -I$(BASEDIR)/open_source/lwip/lwip_adapter/liteos_207/src/include
INCLUDES += -I$(BASEDIR)/kernel/liteos/liteos_v208.5.0/Huawei_LiteOS/compat/linux/include
INCLUDES += -I$(BASEDIR)/kernel/liteos/liteos_v208.5.0/Huawei_LiteOS/lib/liteos_libc/include/

INCLUDES += -I$(BASEDIR)/kernel/osal/include
INCLUDES += -I$(BASEDIR)/kernel/osal/include/lock
INCLUDES += -I$(BASEDIR)/kernel/osal/include/schedule
INCLUDES += -I$(BASEDIR)/kernel/osal/include/memory


INCLUDES += -I$(BASEDIR)/application/samples/bt/ble/ble_wifi_cfg_client/inc
INCLUDES += -I$(BASEDIR)/application/samples/bt/ble/ble_wifi_cfg_server/inc

# -------------------------------------------------------------------
# CHIP compile options
# -------------------------------------------------------------------
CFLAGS =
CFLAGS += -D__LITEOS__
CFLAGS += -DCHIP_WS63=1
CFLAGS += -DCHIP_PROJECT=1
CFLAGS += -DCHIP_HAVE_CONFIG_H=1
CFLAGS += -DCONFIG_SUPPORT_MATTER
CFLAGS += -DCFG_MBEDTLS=1
CFLAGS += -DLWIP_IPV6=1
CFLAGS += -DLWIP_IPV6_MLD=1

CFLAGS += -DLWIP_PBUF_FROM_CUSTOM_POOLS=0
CFLAGS += -DLWIP_CONFIG_FILE=\"lwip/lwipopts_default.h\"

CFLAGS += -DMBEDTLS_USER_CONFIG_FILE=\"mbedtls_platform_hardware_config.h\"
CFLAGS += -DCHIP_CONFIG_SHA256_CONTEXT_SIZE=344

CFLAGS += -DCHIP_DEVICE_LAYER_NONE=0
CFLAGS += -DCHIP_SYSTEM_CONFIG_USE_ZEPHYR_NET_IF=0
CFLAGS += -DCHIP_SYSTEM_CONFIG_USE_BSD_IFADDRS=0
CFLAGS += -DCHIP_SYSTEM_CONFIG_USE_ZEPHYR_SOCKET_EXTENSIONS=0

CFLAGS += -DCHIP_SYSTEM_CONFIG_USE_NETWORK_FRAMEWORK=0
CFLAGS += -DCHIP_ADDRESS_RESOLVE_IMPL_INCLUDE_HEADER="<lib/address_resolve/AddressResolve_DefaultImpl.h>"
CFLAGS += -DLOSCFG_BASE_CORE_TICK_PER_SECOND=100
CFLAGS += -DCHIP_CONFIG_PERSISTED_COUNTER_DEBUG_LOGGING=1

#CFLAGS += -fno-omit-frame-pointer
########################################## common flags #######################################
CFLAGS += -Os
CFLAGS +=-pipe
CFLAGS +=-fno-builtin
CFLAGS +=-fno-exceptions
CFLAGS +=-fno-unwind-tables
CFLAGS +=-fno-common
CFLAGS +=-freg-struct-return
CFLAGS +=-fno-strict-aliasing
CFLAGS +=-falign-functions=2
CFLAGS +=-fdata-sections
CFLAGS +=-ffunction-sections
CFLAGS +=-fno-aggressive-loop-optimizations
CFLAGS += -fshort-enums

########################################## linx131 flags ########################################
CFLAGS +=-mabi=ilp32f
CFLAGS +=-march=rv32imfc
CFLAGS +=-Wa,-enable-c-lbu-sb
CFLAGS +=-Wa,-enable-c-lhu-sh
CFLAGS +=-mpush-pop
CFLAGS +=-msmall-data-limit=0
CFLAGS +=-fimm-compare
CFLAGS +=-femit-muliadd
CFLAGS +=-fmerge-immshf
CFLAGS +=-femit-uxtb-uxth
CFLAGS +=-femit-lli
CFLAGS +=-fldm-stm-optimize
CFLAGS +=-femit-clz

########################################## hcc optimize flags #####################################
CFLAGS +=-madjust-regorder
CFLAGS +=-madjust-const-cost
CFLAGS +=-freorder-commu-args
CFLAGS +=-fimm-compare-expand
CFLAGS +=-frmv-str-zero
CFLAGS +=-mfp-const-opt
CFLAGS +=-frtl-sequence-abstract
CFLAGS +=-frtl-hoist-sink
CFLAGS +=-fsafe-alias-multipointer
CFLAGS +=-finline-optimize-size
CFLAGS +=-fmuliadd-expand
CFLAGS +=-mlli-expand
CFLAGS +=-Wa,-mcjal-expand
CFLAGS +=-foptimize-reg-alloc
CFLAGS +=-fsplit-multi-zero-assignments
CFLAGS +=-floop-optimize-size
CFLAGS +=-Wa,-mlli-relax
CFLAGS +=-mpattern-abstract
CFLAGS +=-foptimize-pro-and-epilogue
CFLAGS +=-Wno-shadow



CXXFLAGS =

CXXFLAGS += -Wno-conversion
CXXFLAGS += -Wno-sign-compare
CXXFLAGS += -Wno-unused-function
CXXFLAGS += -Wno-unused-but-set-variable
CXXFLAGS += -Wno-unused-variable
CXXFLAGS += -Wno-unused-parameter
CXXFLAGS += -Wno-literal-suffix
CXXFLAGS += -fno-rtti
CXXFLAGS += -fno-exceptions
CXXFLAGS += -fpermissive

CHIP_CFLAGS = $(CFLAGS)
CHIP_CFLAGS += $(INCLUDES)

CHIP_CXXFLAGS += $(CFLAGS)
CHIP_CXXFLAGS += $(CXXFLAGS)
CHIP_CXXFLAGS += $(INCLUDES)


export CHIP_ROOT_ENV=$(CHIP_DIR)
export BUILD_ROOT_ENV=$(CHIP_DIR)/build

#*****************************************************************************#
#                        RULES TO GENERATE libCHIP.a                          #
#*****************************************************************************#

# Define the Rules to build the core targets
all: CHIP_CORE

CHIP_CORE:
	@echo "target=$(TARGET)"
	@echo "toolchain=$(TOOLCHAIN_PATH) prefix=$(TOOLCHAIN_PREFIX)"
	@echo "base_dir=$(BASEDIR)"
	@echo "config=$(CONFIG_DIR)"
	@echo "output_dir=$(OUTPUT_DIR)"
	if [ ! -d $(OUTPUT_DIR) ]; then \
		mkdir -p $(OUTPUT_DIR); mkdir -p $(OUTPUT_DIR)/app_obj;\
	fi
	if [ ! -d $(OUTPUT_DIR)/app_obj ]; then \
		mkdir -p $(OUTPUT_DIR)/app_obj;\
	fi
	@echo "Compiling CHIP SDK static library"
	@echo                                   > $(OUTPUT_DIR)/args.gn
	@echo "import(\"//args.gni\")"          >> $(OUTPUT_DIR)/args.gn
	@echo target_cflags_c  = [$(foreach word,$(CHIP_CFLAGS),\"$(word)\",)] | sed -e 's/=\"/=\\"/g;s/\"\"/\\"\"/g;'  >> $(OUTPUT_DIR)/args.gn
	@echo target_cflags_cc = [$(foreach word,$(CHIP_CXXFLAGS),\"$(word)\",)] | sed -e 's/=\"/=\\"/g;s/\"\"/\\"\"/g;'   >> $(OUTPUT_DIR)/args.gn
	@echo hisi_ar = \"$(AR)\"    >> $(OUTPUT_DIR)/args.gn
	@echo hisi_cc = \"$(CC)\"   >> $(OUTPUT_DIR)/args.gn
	@echo hisi_cxx = \"$(CXX)\"  >> $(OUTPUT_DIR)/args.gn

	cd $(CHIP_DIR)/examples/lighting-app/hisilicon && gn gen --check --fail-on-unused-args $(OUTPUT_DIR)/out/$(TARGET) && cp $(OUTPUT_DIR)/args.gn $(OUTPUT_DIR)/out/$(TARGET)/
	cd $(CHIP_DIR)/examples/lighting-app/hisilicon; ninja -C $(OUTPUT_DIR)/out/$(TARGET)

.PHONY: clean
clean:
	rm -rf $(OUTPUT_DIR)/

