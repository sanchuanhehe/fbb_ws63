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

#ifndef AHT20_TEST_H
#define AHT20_TEST_H

/***************************************************\
* 环境监测数据
\***************************************************/
typedef struct {
    float temperature; // 温度
    float humidity;    // 湿度
} environment_msg;

void aht20_test_task(environment_msg *msg);
void temp_hum_chinese(void);
void aht20_init(void);

#endif // AHT20_H