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

#include "bsp.h"

void my_gpio_init(pin_t gpio)
{
    uapi_pin_set_mode(gpio, HAL_PIO_FUNC_GPIO);

    uapi_gpio_set_dir(gpio, GPIO_DIRECTION_OUTPUT);
    uapi_gpio_set_val(gpio, GPIO_LEVEL_LOW);
}

gpio_level_t my_io_readval(pin_t gpio)
{
    return uapi_gpio_get_output_val(gpio);
}

void my_io_setval(pin_t gpio, gpio_level_t level)
{
    uapi_gpio_set_val(gpio, level);
}