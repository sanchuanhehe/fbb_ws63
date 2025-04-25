/**
 * Copyright (c) Adragon
 *
 * Description: WiFi STA and HTTP Get to get weather forecasts. \n
 *              This file implements a HTTP Get and Read HTTP Response example.
 *
 *
 * History: \n
 * 2025-03-18, Create file. \n
 */
#include "http.h"
#include "weather.h"
#include "wifista.h"
static const char *g_request = "GET /observe?city=CH280601&key=62w9bk1okpme4k59 HTTP/1.1\r\n"
                               "Host: api.yytianqi.com\r\n"
                               "Connection: close\r\n"
                               "\r\n";

/* HTTP Get请求 */
void http_client_get(void *param)
{
    param = param; //
    struct sockaddr_in addr = {0};
    int s, r;
    char recv_buf[HTTPC_DEMO_RECV_BUFSIZE];

    osal_printk("*****Connect to WiFi: ");
    osal_printk(CONFIG_WIFI_SSID);
    osal_printk("*****\r\n");
    wifi_connect(CONFIG_WIFI_SSID, CONFIG_WIFI_PWD);

    addr.sin_family = AF_INET;
    addr.sin_port = PP_HTONS(CONFIG_SERVER_PORT);
    addr.sin_addr.s_addr = inet_addr(CONFIG_SERVER_IP);
    s = socket(AF_INET, SOCK_STREAM, 0);
    osal_printk("s = %d\r\n", s);
    if (s < 0)
    {
        return;
    }

    // socket连接服务器
    osal_printk("NO1:... allocated socket\r\n");
    if (connect(s, (struct sockaddr *)&addr, sizeof(addr)) != 0)
    {
        osal_printk("... socket connect failed errno=%d", errno);
        lwip_close(s);
        return;
    }
    osal_printk("NO2:... connected\r\n");
    // 发送HTTP GET请求
    if (lwip_write(s, g_request, strlen(g_request)) < 0)
    {
        lwip_close(s);
        return;
    }
    osal_printk("NO3:... socket send success\r\n");

    /* 5S Timeout */
    struct timeval receiving_timeout;
    receiving_timeout.tv_sec = 5;
    receiving_timeout.tv_usec = 0;
    if (setsockopt(s, SOL_SOCKET, SO_RCVTIMEO, &receiving_timeout, sizeof(receiving_timeout)) < 0)
    {
        osal_printk("... failed to set socket receiving timeout\r\n");
        lwip_close(s);
        return;
    }
    osal_printk("NO4:... set socket receiving timeout success\r\n");

    /* Read HTTP response */
    do
    {
        (void)uapi_watchdog_kick();
        (void)memset_s(recv_buf, sizeof(recv_buf), 0, sizeof(recv_buf));
        r = lwip_read(s, recv_buf, sizeof(recv_buf) - 1);
        osal_printk("r = %d\r\n", r);
        if (r <= 0)
        {
            osal_printk("lwip_read Done!\r\n");
            break;
        }

        for (int i = 0; i < r; i++)
        {
            osal_printk("%c", recv_buf[i]);
        }

        // 解析并输出天气、风力、风向、城市名称、时间和相对湿度
        parse_weather_data(recv_buf);
    } while (r > 0);

    osal_printk("...done reading from socket. Last read return=%d, errno=%d\r\n", r, errno);
    lwip_close(s);

    return;
}

static void tcp_client_sample_entry(void)
{
    osThreadAttr_t attr;
    attr.name = "tcp_client_sample_task";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = TCP_CLIENT_TASK_STACK_SIZE;
    attr.priority = TCP_CLIENT_TASK_PRIO;
    if (osThreadNew((osThreadFunc_t)http_client_get, NULL, &attr) == NULL)
    {
        osal_printk("Create tcp_client_get fail.\r\n");
    }
    osal_printk("Create tcp_client_get succ.\r\n");
}

app_run(tcp_client_sample_entry);