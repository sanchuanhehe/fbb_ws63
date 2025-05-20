/*
 * Copyright (c) 2024 Beijing HuaQingYuanJian Education Technology Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#ifndef WIFI_CONNECT_H
#define WIFI_CONNECT_H
#include "errcode.h"

#define CONFIG_WIFI_SSID "test"  // 要连接的WiFi 热点账号
#define CONFIG_WIFI_PWD "123456" // 要连接的WiFi 热点密码

errcode_t wifi_connect(void);
#endif