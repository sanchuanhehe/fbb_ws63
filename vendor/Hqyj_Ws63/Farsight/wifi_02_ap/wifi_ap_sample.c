/*
 * Copyright (c) 2024 HiSilicon Technologies CO., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "lwip/netifapi.h"
#include "wifi_hotspot.h"
#include "wifi_hotspot_config.h"
#include "stdlib.h"
#include "uart.h"
#include "cmsis_os2.h"
#include "app_init.h"
#include "soc_osal.h"

#define DELAY_TIME_MS 100

/*****************************************************************************
  SoftAP sample用例
*****************************************************************************/
errcode_t example_softap_function(td_void)
{
    /* SoftAp接口的信息 */
    td_char ssid[WIFI_MAX_SSID_LEN] = "HQYJ_HHQYJ_WS63";
    td_char pre_shared_key[WIFI_MAX_KEY_LEN] = "123456789";
    softap_config_stru hapd_conf = {0};
    softap_config_advance_stru config = {0};
    td_char ifname[WIFI_IFNAME_MAX_SIZE + 1] = "ap0"; /* 创建的SoftAp接口名 */
    struct netif *netif_p = TD_NULL;
    ip4_addr_t st_gw;
    ip4_addr_t st_ipaddr;
    ip4_addr_t st_netmask;
    IP4_ADDR(&st_ipaddr, 192, 168, 5, 1);    /* IP地址设置：192.168.5.1 */
    IP4_ADDR(&st_netmask, 255, 255, 255, 0); /* 子网掩码设置：255.255.255.0 */
    IP4_ADDR(&st_gw, 192, 168, 5, 2);        /* 网关地址设置：192.168.5.2 */

    /* 配置SoftAp基本参数 */
    (td_void) memcpy_s(hapd_conf.ssid, sizeof(hapd_conf.ssid), ssid, sizeof(ssid));
    (td_void) memcpy_s(hapd_conf.pre_shared_key, WIFI_MAX_KEY_LEN, pre_shared_key, WIFI_MAX_KEY_LEN);
    hapd_conf.security_type = 3; /* 3: 加密方式设置为WPA_WPA2_PSK */
    hapd_conf.channel_num = 6;   /* 13：工作信道设置为6信道 */
    hapd_conf.wifi_psk_type = 0;

    /* 配置SoftAp网络参数 */
    config.beacon_interval = 100; /* 100：Beacon周期设置为100ms */
    config.dtim_period = 2;       /* 2：DTIM周期设置为2 */
    config.gi = 0;                /* 0：short GI默认关闭 */
    config.group_rekey = 86400;   /* 86400：组播秘钥更新时间设置为1天 */
    config.protocol_mode = 4;     /* 4：协议类型设置为802.11b + 802.11g + 802.11n + 802.11ax */
    config.hidden_ssid_flag = 1;  /* 1：不隐藏SSID */
    if (wifi_set_softap_config_advance(&config) != 0) {
        return ERRCODE_FAIL;
    }
    /* 启动SoftAp接口 */
    if (wifi_softap_enable(&hapd_conf) != 0) {
        return ERRCODE_FAIL;
    }
    /* 配置DHCP服务器 */
    netif_p = netif_find(ifname);
    if (netif_p == TD_NULL) {
        (td_void) wifi_softap_disable();
        return ERRCODE_FAIL;
    }
    if (netifapi_netif_set_addr(netif_p, &st_ipaddr, &st_netmask, &st_gw) != 0) {
        (td_void) wifi_softap_disable();
        return ERRCODE_FAIL;
    }
    if (netifapi_dhcps_start(netif_p, NULL, 0) != 0) {
        (td_void) wifi_softap_disable();
        return ERRCODE_FAIL;
    }
    printf("SoftAp start success.\r\n");
    return ERRCODE_SUCC;
}

int softap_sample_init(const char *argument)
{
    argument = argument;
    /* 等待wifi初始化完成 */
    while (wifi_is_wifi_inited() == 0) {
        osDelay(DELAY_TIME_MS);
    }
    if (example_softap_function() != 0) {
        printf("softap_function fail.\r\n");
        return -1;
    }
    return 0;
}

static void softap_sample(void)
{
    osThreadAttr_t attr;
    attr.name = "softap_sample_task";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = 0x1000;
    attr.priority = osPriorityNormal;
    if (osThreadNew((osThreadFunc_t)softap_sample_init, NULL, &attr) == NULL) {
        printf("Create softap_sample_task fail.\r\n");
    }
    printf("Create softap_sample_task succ.\r\n");
}

/* Run the sample. */
app_run(softap_sample);