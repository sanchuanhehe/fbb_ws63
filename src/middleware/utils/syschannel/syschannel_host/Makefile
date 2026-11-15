WLAN_DIR := $(dir $(abspath $(lastword $(MAKEFILE_LIST))))

include $(WLAN_DIR)/env_config.mk

ifeq ($(CFG_3518EV300), y)
ARCH ?= arm
KDIR ?= ${WLBUS_SDIO_KERNEL_DIR}
WLAN_CFLAGS += -DSYSCHANNEL_LITTLE_ENDIAN=1
WLAN_CFLAGS += -DSYSCHANNEL_BIG_ENDIAN=2
WLAN_CFLAGS += -DSYSCHANNEL_ENDIAN=SYSCHANNEL_LITTLE_ENDIAN
WLAN_CFLAGS += -DETH_PAD_SIZE=2
CROSS_COMPILE ?= arm-himix100-linux-
#CROSS_COMPILE ?= x86_64-linux-gnu-
endif

ifeq ($(CFG_T23), y)
ARCH ?= mips
KDIR ?= /home/shell/kernel_t23
WLAN_CFLAGS += -DSYSCHANNEL_LITTLE_ENDIAN=1
WLAN_CFLAGS += -DSYSCHANNEL_BIG_ENDIAN=2
WLAN_CFLAGS += -DSYSCHANNEL_ENDIAN=SYSCHANNEL_LITTLE_ENDIAN
WLAN_CFLAGS += -DETH_PAD_SIZE=2
CROSS_COMPILE ?= mips-linux-gnu-
endif
MAIN_PATH :=
OAL_PATH := oal/linux/
NETLINK_PATH := wal/linux/
WAL_PATH := wal/linux/
SYSCHANNEL_PATH := channel_host/
SEC_PATH := ../../../../open_source/libboundscheck/src/
COM_PATH := ../syschannel_common/
OSAL_ADAPT_PATH := ../../../../kernel/osal_adapt/src/
OSAL_PATH := ../../../../kernel/osal/src/linux/kernel/
OSAL_MEDIA_PATH := ../../../../kernel/osal/src/linux/kernel/media/
HCC_PATH := ../../hcc/comm/
HCC_DRV_PATH := ../../hcc/host/
TOP_DIR := $(WLAN_DIR)/../../../../
WLAN_CFLAGS += -std=gnu99 -Wno-declaration-after-statement -fsigned-char -freg-struct-return -Wtrampolines -Wfloat-equal -fvisibility=hidden
ifeq ($(CFG_3518EV300), y)
WLAN_CFLAGS += -Wdate-time
endif
#linux系统
WLAN_CFLAGS += -D_PRE_OS_VERSION_LINUX=1
WLAN_CFLAGS += -D_PRE_OS_VERSION_LITEOS=2
WLAN_CFLAGS += -D_PRE_OS_VERSION=_PRE_OS_VERSION_LINUX
#内存零拷贝宏
WLAN_CFLAGS += -DPBUF_ZERO_COPY_RESERVE=80
# osal依赖
WLAN_CFLAGS += -I$(TOP_DIR)/kernel/osal/include
WLAN_CFLAGS += -I$(TOP_DIR)/middleware/utils/common_headers/osal
WLAN_CFLAGS += -I$(TOP_DIR)/kernel/osal_adapt/inc
WLAN_CFLAGS += -I$(TOP_DIR)/kernel/osal/src/linux/kernel
WLAN_CFLAGS += -I$(TOP_DIR)/kernel/osal/src/linux/kernel/media
# securec依赖
WLAN_CFLAGS += -I$(TOP_DIR)/open_source/libboundscheck/include

#hcc依赖
WLAN_CFLAGS += -DCONFIG_HCC_SUPPORT_REG_OPT=1
WLAN_CFLAGS += -I$(TOP_DIR)/include
WLAN_CFLAGS += -I$(TOP_DIR)/middleware/utils/hcc/inc
WLAN_CFLAGS += -I$(TOP_DIR)/middleware/utils/hcc/cfg
WLAN_CFLAGS += -I$(TOP_DIR)/middleware/utils/hcc/comm
WLAN_CFLAGS += -I$(TOP_DIR)/middleware/utils/hcc/host
WLAN_CFLAGS += -I$(TOP_DIR)/middleware/utils/common_headers/native
main-objs += main.o
main-objs := $(addprefix $(MAIN_PATH),$(main-objs))

oal-objs += oal_netbuf.o oal_netdev.o
oal-objs := $(addprefix $(OAL_PATH),$(oal-objs))

netlink-objs  += wal_netlink.o
netlink-objs := $(addprefix $(NETLINK_PATH),$(netlink-objs))

wal-objs += wal_net.o
wal-objs := $(addprefix $(WAL_PATH),$(wal-objs))

com-objs := syschannel_common.o
com-objs := $(addprefix $(COM_PATH),$(com-objs))

syschannel-objs := syschannel_host_adapt.o
syschannel-objs := $(addprefix $(SYSCHANNEL_PATH),$(syschannel-objs))

sec-objs += memcpy_s.o memmove_s.o memset_s.o strcat_s.o strcpy_s.o strncat_s.o strncpy_s.o strtok_s.o \
            snprintf_s.o vsnprintf_s.o secureprintoutput_a.o sscanf_s.o vsscanf_s.o secureinput_a.o
sec-objs := $(addprefix $(SEC_PATH),$(sec-objs))

osal_adapt-objs += osal_adapt_timer.o
osal_adapt-objs := $(addprefix $(OSAL_ADAPT_PATH),$(osal_adapt-objs))

osal-objs += osal_task.o osal_spinlock.o osal_wait.o osal_atomic.o osal_addr.o osal_atomic.o
osal-objs += osal_barrier.o osal_bitmap.o osal_cache.o osal_completion.o osal_debug.o osal_delayedwork.o
osal-objs += osal_interrupt.o osal_mutex.o osal_rwlock.o osal_semaphore.o osal_spinlock.o
osal-objs += osal_task.o osal_wait.o osal_workqueue.o
osal-objs += osal_timer.o osal_string.o osal_proc.o
osal-objs := $(addprefix $(OSAL_PATH),$(osal-objs))

osal_media-objs += media.o media_base.o
osal_media-objs := $(addprefix $(OSAL_MEDIA_PATH),$(osal_media-objs))

hcc-objs := hcc.o hcc_service.o hcc_adapt.o hcc_bus.o hcc_channel.o hcc_dfx.o hcc_flow_ctrl.o hcc_list.o  hcc_rom_callback.o hcc_test.o hcc_test_service.o
hcc-objs := $(addprefix $(HCC_PATH),$(hcc-objs))

hcc_drv-objs := hcc_sdio_host.o hcc_test_host.o hcc_test_linux.o
hcc_drv-objs := $(addprefix $(HCC_DRV_PATH),$(hcc_drv-objs))

WLAN_OBJS += $(main-objs) $(oal-objs) $(netlink-objs) $(wal-objs) $(syschannel-objs) $(sec-objs) $(com-objs) $(osal_adapt-objs) $(hcc-objs) $(hcc_drv-objs)
WLAN_OBJS += $(osal-objs) $(osal_media-objs)
EXTRA_CFLAGS += $(WLAN_CFLAGS)
EXTRA_CFLAGS += -DCONFIG_HCC_SUPPORT_DFX \
                -DCONFIG_HCC_SUPPORT_FLOW_CONTRL \
                -DCONFIG_HCC_SUPPORT_UNC_POOL \
                -DCONFIG_HCC_SUPPORT_FLOW_CONTRL_ACTIVE \
                -DCONFIG_HCC_SUPPORT_SDIO \
                -DCONFIG_HCC_SUPPORT_TEST \
                -DCONFIG_HCC_SUPPORT_TEST_SYSCH \
                -DCONFIG_HCC_CONFIG_DATA_FC

PWD := $(shell pwd)
USER_MODULE_NAME := syschannel
MODULE_NAME := $(USER_MODULE_NAME)
$(MODULE_NAME)-y += $(WLAN_OBJS)
obj-m = $(MODULE_NAME).o

#==================================================================
#                          modules
#==================================================================
.PHONY: all sample sample_clean clean

all:
	make ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) -C $(KDIR) M=$(PWD) modules

sample:
	$(MAKE) -C sample all

sample_clean:
	$(MAKE) -C sample clean

clean:
	rm -rf $(COBJS) $(MODULE_NAME).o $(MODULE_NAME).ko
	rm -rf ./*.ko
	rm -rf ./*.o
	rm -rf ./.*.cmd
	rm -rf ./*.symvers
	rm -rf ./*.order
	rm -rf ./*.mod.c
	rm -rf ./oal/linux/*.o
	rm -rf ./oal/linux/.*.cmd
	rm -rf ./wal/*.o
	rm -rf ./wal/.*.cmd
	rm -rf ./wal/linux/*.o
	rm -rf ./wal/linux/.*.cmd
	rm -rf ./$(SEC_PATH)*.o
	rm -rf ./$(SEC_PATH).*.cmd
	rm -rf ./$(OSAL_ADAPT_PATH)*.o
	rm -rf ./$(OSAL_ADAPT_PATH).*.cmd
	rm -rf ./$(OSAL_PATH)*.o
	rm -rf ./$(OSAL_PATH).*.cmd
	rm -rf ./$(OSAL_MEDIA_PATH)*.o
	rm -rf ./$(OSAL_MEDIA_PATH).*.cmd
	rm -rf ./$(HCC_PATH)*.o
	rm -rf ./$(HCC_PATH).*.cmd
	rm -rf ./$(HCC_DRV_PATH)*.o
	rm -rf ./$(HCC_DRV_PATH).*.cmd
	rm -rf ./../syschannel_common/*.o
	rm -rf ./../syschannel_common/.*.cmd
	rm -rf ./channel_host/*.o
	rm -rf ./channel_host/.*.cmd
	rm -rf .tmp_versions
