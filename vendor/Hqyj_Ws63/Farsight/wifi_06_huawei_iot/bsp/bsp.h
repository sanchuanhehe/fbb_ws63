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

#ifndef BSP_H
#define BSP_H

#include "pinctrl.h"
#include "gpio.h"
#include "platform_core_rom.h"

#define SENSOR_IO GPIO_10

void my_gpio_init(pin_t gpio);

gpio_level_t my_io_readval(pin_t gpio);

void my_io_setval(pin_t gpio, gpio_level_t level);
#endif