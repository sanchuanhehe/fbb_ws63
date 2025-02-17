#ifndef _WIFI_CONNECT_
#define _WIFI_CONNECT_
 
#define CONFIG_WIFI_SSID            "AI_DEV"                              // 要连接的WiFi 热点账号
#define CONFIG_WIFI_PWD             "HQYJ12345678"                        // 要连接的WiFi 热点密码

errcode_t wifi_connect(void);
#endif