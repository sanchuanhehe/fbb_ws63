/* 头文件包含 */

#include "wal_net.h"
#if defined(_PRE_OS_VERSION_LINUX) && defined(_PRE_OS_VERSION) && (_PRE_OS_VERSION == _PRE_OS_VERSION_LINUX)
#include "wal_netlink.h"
#endif
#include "hcc_if.h"
#include "syschannel_common.h"
#include "syschannel_host_adapt.h"
#include "osal_def.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/* 函数定义 */
/* insmod syschannel.ko 入口函数 */
osal_s32 wlan_init(void)
{
    osal_s32 ret;

    osal_printk("syschannel start run\r\n");
#if defined(_PRE_OS_VERSION_LINUX) && defined(_PRE_OS_VERSION) && (_PRE_OS_VERSION == _PRE_OS_VERSION_LINUX)
    ret = oal_netlink_init();
    if (ret != OSAL_OK) {
        osal_printk("wlan_init:: oal_netlink_init FAILED[%d]\r\n", ret);
        return OSAL_NOK;
    }
    hcc_test_cmd_ctrl_init();
#endif

    ret = syschannel_host_init();
    if (ret != OSAL_OK) {
        goto syschannel_host_init_fail;
    }

    ret = netdev_register();
    if (ret != OSAL_OK) {
        osal_printk("wlan_init:: netdev_register failed\r\n");
        return ret;
    }

    osal_printk("syschannel.ko insmod SUCCESSFULLY\r\n");
    return OSAL_OK;

syschannel_host_init_fail:
#if defined(_PRE_OS_VERSION_LINUX) && defined(_PRE_OS_VERSION) && (_PRE_OS_VERSION == _PRE_OS_VERSION_LINUX)
    oal_netlink_deinit();
#endif
    return OSAL_NOK;
}

/* rmmod syschannel.ko 入口函数 */
void wlan_deinit(void)
{
    netdev_unregister();
    syschannel_host_exit();
#if defined(_PRE_OS_VERSION_LINUX) && defined(_PRE_OS_VERSION) && (_PRE_OS_VERSION == _PRE_OS_VERSION_LINUX)
    hcc_test_cmd_ctrl_deinit();
    oal_netlink_deinit();
#endif
    return;
}

OSAL_MODULE_LICENSE("GPL");
OSAL_MODULE_VERSION("V1.0.0_000.20200902");
OSAL_MODULE_DESCRIPTION("Wlan Driver");
OSAL_MODULE_AUTHOR("Wifi Team");
osal_module_init(wlan_init);
osal_module_exit(wlan_deinit);

#ifdef __cplusplus
#if __cplusplus
    }
#endif
#endif