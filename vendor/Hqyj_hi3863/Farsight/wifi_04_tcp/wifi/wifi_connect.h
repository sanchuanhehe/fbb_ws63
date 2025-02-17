#ifndef _WIFI_CONNECT_
#define _WIFI_CONNECT_
 
#define CONFIG_WIFI_SSID            "HQYJ_H3863"                              // 要连接的WiFi 热点账号
#define CONFIG_WIFI_PWD             "123456789"                        // 要连接的WiFi 热点密码
#define CONFIG_SERVER_IP           "192.168.184.230"                       // 要连接的服务器IP
#define CONFIG_SERVER_PORT          6789                                // 要连接的服务器端口
errcode_t wifi_connect(void);
#endif