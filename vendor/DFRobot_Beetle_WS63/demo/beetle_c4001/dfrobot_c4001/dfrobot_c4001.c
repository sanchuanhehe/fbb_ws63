/*!
 * @file dfrobot_c4001.c
 * @brief Define the basic structure of the DFRobot_C4001 class, the implementation of the basic methods
 * @copyright    Copyright (c) 2025 DFRobot Co.Ltd (http://www.dfrobot.com)
 * @license The MIT License (MIT)
 * @author [Martin](Martin@dfrobot.com)
 * @version V1.0
 * @date 2025-09-29
 * @url https://github.com/DFRobot/DFRobot_C4001
 */
#include "dfrobot_c4001.h"

static s_private_data_t g_buffer;

#define TIME_OUT C4001_TIME_OUT_MS ///< time out
#define SENSOR_UART_ID 2           // 替换成你初始化时使用的 UART ID
#define DELAY_MS C4001_DELAY_MS_BASE

#define THE_UART_TRANSFER_SIZE C4001_UART_TRANSFER_SIZE
static uint8_t g_app_uart_rx_buff[THE_UART_TRANSFER_SIZE] = {0};
static uart_buffer_config_t g_app_uart_buffer_config = {.rx_buffer = g_app_uart_rx_buff,
                                                        .rx_buffer_size = THE_UART_TRANSFER_SIZE};

static void write_reg(uint8_t reg, const uint8_t *data, uint8_t len)
{
    (void)reg;
    if (len > sizeof(g_app_uart_rx_buff)) {
        osal_printk("UART write error: len too large\r\n");
        return;
    }

    // 拷贝数据到全局 buffer
    memcpy(g_app_uart_rx_buff, data, len);

    // 调用底层写接口发送
    int ret = uapi_uart_write(SENSOR_UART_ID, g_app_uart_rx_buff, len, 0);
    if (ret < 0) {
        osal_printk("UART write error: %d\r\n", ret);
    }
}

static int16_t read_reg(uint8_t reg, uint8_t *data, uint8_t len)
{
    (void)reg;

    if (len > sizeof(g_app_uart_rx_buff)) {
        osal_printk("UART read error: len too large\r\n");
        return -1;
    }

    uint16_t received = 0;
    uint32_t start = uapi_systick_get_ms();

    while (uapi_systick_get_ms() - start < TIME_OUT) {
        // 把数据直接读到 g_app_uart_rx_buff
        int ret = uapi_uart_read(SENSOR_UART_ID, g_app_uart_rx_buff + received, len - received, 0);
        if (ret > 0) {
            received += ret;
            if (received >= len) {
                break; // 收够数据
            }
        }
    }

    if (received > 0) {
        // 拷贝到用户缓冲
        memcpy(data, g_app_uart_rx_buff, received);
    }

    return received;
}

static s_response_data_t analysis_response(uint8_t *data, uint8_t len, uint8_t count)
{
    s_response_data_t response_data;
    uint8_t space[C4001_SPACE_ARRAY_SIZE] = {0};
    uint8_t i = 0;
    uint8_t j = 0;
    for (i = 0; i < len; i++) {
        if (data[i] == C4001_STR_CHAR_R && data[i + C4001_ARRAY_INDEX_1] == C4001_STR_CHAR_E &&
            data[i + C4001_ARRAY_INDEX_2] == C4001_STR_CHAR_S) {
            break;
        }
    }
    if (i == len || i == C4001_ARRAY_INDEX_0) {
        response_data.status = false;
    } else {
        response_data.status = true;
        for (j = 0; i < len; i++) {
            if (data[i] == C4001_STR_CHAR_SPACE) {
                space[j++] = i + C4001_ARRAY_INDEX_1;
            }
        }
        if (j != C4001_ARRAY_INDEX_0) {
            response_data.response1 = atof((const char *)(data + space[C4001_ARRAY_INDEX_0]));
            if (j >= C4001_ARRAY_INDEX_2) {
                response_data.response2 = atof((const char *)(data + space[C4001_ARRAY_INDEX_1]));
            }
            if (count == C4001_ARRAY_INDEX_3) {
                response_data.response3 = atof((const char *)(data + space[C4001_ARRAY_INDEX_2]));
            }
        } else {
            response_data.response1 = 0.0;
            response_data.response2 = 0.0;
        }
    }
    return response_data;
}

static s_all_data_t analysis_data(uint8_t *data, uint8_t len)
{
    s_all_data_t all_data;
    uint8_t location = 0;
    memset(&all_data, 0, sizeof(s_all_data_t));
    for (uint8_t i = 0; i < len; i++) {
        if (data[i] == C4001_STR_CHAR_DOLLAR) {
            location = i;
            break;
        }
    }
    if (location == len) {
        return all_data;
    }
    if (strncmp((const char *)(data + location), "$DFHPD", C4001_STR_DFHPD_LEN) == 0) {
        all_data.sta.work_mode = EXITMODE;
        all_data.sta.work_status = C4001_ARRAY_INDEX_1;
        all_data.sta.init_status = C4001_ARRAY_INDEX_1;
        if (data[location + C4001_STR_POS_DFHPD_EXIST_OFFSET] == '1') {
            all_data.exist = C4001_ARRAY_INDEX_1;
        } else {
            all_data.exist = C4001_ARRAY_INDEX_0;
        }
    } else if (strncmp((const char *)(data + location), "$DFDMD", C4001_STR_DFDMD_LEN) == 0) {
        all_data.sta.work_mode = SPEEDMODE;
        all_data.sta.work_status = C4001_ARRAY_INDEX_1;
        all_data.sta.init_status = C4001_ARRAY_INDEX_1;
        char *token;
        char *parts[C4001_PARTS_ARRAY_SIZE]; // Let's say there are at most 10 parts
        int index = 0;                       // Used to track the number of parts stored
        token = strtok((char *)(data + location), ",");
        while (token != NULL) {
            parts[index] = token; // Stores partial Pointers in an array
            if (index++ > C4001_STR_POS_PARTS_INDEX_MAX) {
                break;
            }
            token = strtok(NULL, ","); // Continue to extract the next section
        }
        all_data.target.number = atoi(parts[C4001_ARRAY_INDEX_1]);
        all_data.target.range = atof(parts[C4001_ARRAY_INDEX_3]) * C4001_RANGE_FROM_METER_MULTIPLIER;
        all_data.target.speed = atof(parts[C4001_ARRAY_INDEX_4]) * C4001_RANGE_FROM_METER_MULTIPLIER;
        all_data.target.energy = atof(parts[C4001_ARRAY_INDEX_5]);
    } else {
    }
    return all_data;
}

static bool sensor_stop(void)
{
    uint8_t len = 0;
    uint8_t temp[C4001_TEMP_BUFFER_SIZE] = {0};
    write_reg(C4001_ARRAY_INDEX_0, (uint8_t *)STOP_SENSOR, strlen(STOP_SENSOR));
    uapi_systick_delay_ms(C4001_DELAY_1000_MS);
    len = read_reg(C4001_ARRAY_INDEX_0, temp, C4001_TEMP_BUFFER_SIZE);
    while (C4001_ARRAY_INDEX_1) {
        if (len != C4001_ARRAY_INDEX_0) {
            if (strstr((const char *)temp, "sensorStop") != NULL) {
                return true;
            }
        }
        memset(temp, 0, C4001_TEMP_BUFFER_SIZE);
        uapi_systick_delay_ms(C4001_DELAY_400_MS);
        write_reg(C4001_ARRAY_INDEX_0, (uint8_t *)STOP_SENSOR, strlen(STOP_SENSOR));
        len = read_reg(C4001_ARRAY_INDEX_0, temp, C4001_TEMP_BUFFER_SIZE);
    }
}

static s_response_data_t write_read_cmd(char *cmd1, uint8_t count)
{
    uint8_t len = 0;
    uint8_t temp[C4001_TEMP_BUFFER_SIZE] = {0};
    s_response_data_t response_data;
    sensor_stop();
    write_reg(C4001_ARRAY_INDEX_0, (uint8_t *)cmd1, strlen(cmd1));
    uapi_systick_delay_ms(C4001_DELAY_100_MS);
    len = read_reg(C4001_ARRAY_INDEX_0, temp, C4001_TEMP_BUFFER_SIZE);
    response_data = analysis_response(temp, len, count);
    uapi_systick_delay_ms(C4001_DELAY_100_MS);
    write_reg(C4001_ARRAY_INDEX_0, (uint8_t *)START_SENSOR, strlen(START_SENSOR));
    uapi_systick_delay_ms(C4001_DELAY_100_MS);
    return response_data;
}

static void write_cmd(char *cmd1, char *cmd2, uint8_t count)
{
    sensor_stop();
    write_reg(C4001_ARRAY_INDEX_0, (uint8_t *)cmd1, strlen(cmd1));
    uapi_systick_delay_ms(C4001_DELAY_100_MS);
    if (count > C4001_ARRAY_INDEX_1) {
        uapi_systick_delay_ms(C4001_DELAY_100_MS);
        write_reg(C4001_ARRAY_INDEX_0, (uint8_t *)cmd2, strlen(cmd2));
        uapi_systick_delay_ms(C4001_DELAY_100_MS);
    }
    write_reg(C4001_ARRAY_INDEX_0, (uint8_t *)SAVE_CONFIG, strlen(SAVE_CONFIG));
    uapi_systick_delay_ms(C4001_DELAY_100_MS);
    write_reg(C4001_ARRAY_INDEX_0, (uint8_t *)START_SENSOR, strlen(START_SENSOR));
    uapi_systick_delay_ms(C4001_DELAY_100_MS);
}

void dfrobot_c4001_init(uint32_t baud, uint8_t txpin, uint8_t rxpin, uint8_t uart_id)
{
    // 1. 配置引脚
#if defined(CONFIG_PINCTRL_SUPPORT_IE)
    uapi_pin_set_ie(rxpin, PIN_IE_1);
#endif
    uapi_pin_set_mode(txpin, PIN_MODE_2);
    uapi_pin_set_mode(rxpin, PIN_MODE_2);

    // 2. 配置 UART 属性
    uart_attr_t attr = {
        .baud_rate = baud, .data_bits = UART_DATA_BIT_8, .stop_bits = UART_STOP_BIT_1, .parity = UART_PARITY_NONE};

    uart_pin_config_t pin_config = {.tx_pin = txpin, .rx_pin = rxpin, .cts_pin = PIN_NONE, .rts_pin = PIN_NONE};

    uapi_uart_deinit(uart_id);
    uapi_uart_init(uart_id, &pin_config, &attr, NULL, &g_app_uart_buffer_config);
}

s_sensor_status_t get_status(void)
{
    s_sensor_status_t data;
    uint8_t temp[C4001_STATUS_BUFFER_SIZE] = {0};
    uint8_t len = 0;

    s_all_data_t all_data;
    read_reg(C4001_ARRAY_INDEX_0, temp, C4001_STATUS_BUFFER_SIZE);
    write_reg(C4001_ARRAY_INDEX_0, (uint8_t *)START_SENSOR, strlen(START_SENSOR));
    while (len == C4001_ARRAY_INDEX_0) {
        uapi_systick_delay_ms(C4001_DELAY_1000_MS);
        len = read_reg(C4001_ARRAY_INDEX_0, temp, C4001_STATUS_BUFFER_SIZE);
        all_data = analysis_data(temp, len);
    }
    data.work_status = all_data.sta.work_status;
    data.work_mode = all_data.sta.work_mode;
    data.init_status = all_data.sta.init_status;

    return data;
}

bool motion_detection(void)
{
    static bool old = false;
    uint8_t status = C4001_ARRAY_INDEX_0;
    uint8_t len = 0;
    uint8_t temp[C4001_STATUS_BUFFER_SIZE] = {0};
    s_all_data_t data;
    len = read_reg(C4001_ARRAY_INDEX_0, temp, C4001_STATUS_BUFFER_SIZE);
    data = analysis_data(temp, len);
    if (data.exist) {
        old = (bool)status;
        return (bool)data.exist;
    } else {
        return (bool)old;
    }
}

void set_sensor(e_set_mode_t mode)
{
    if (mode == STARTSEN) {
        write_reg(C4001_ARRAY_INDEX_0, (uint8_t *)START_SENSOR, strlen(START_SENSOR));
        uapi_systick_delay_ms(C4001_DELAY_200_MS); // must timer
    } else if (mode == STOPSEN) {
        write_reg(C4001_ARRAY_INDEX_0, (uint8_t *)STOP_SENSOR, strlen(STOP_SENSOR));
        uapi_systick_delay_ms(C4001_DELAY_200_MS); // must timer
    } else if (mode == RESETSEN) {
        write_reg(C4001_ARRAY_INDEX_0, (uint8_t *)RESET_SENSOR, strlen(RESET_SENSOR));
        uapi_systick_delay_ms(C4001_DELAY_1500_MS); // must timer
    } else if (mode == SAVEPARAMS) {
        write_reg(C4001_ARRAY_INDEX_0, (uint8_t *)STOP_SENSOR, strlen(STOP_SENSOR));
        uapi_systick_delay_ms(C4001_DELAY_200_MS); // must timer
        write_reg(C4001_ARRAY_INDEX_0, (uint8_t *)SAVE_CONFIG, strlen(SAVE_CONFIG));
        uapi_systick_delay_ms(C4001_DELAY_800_MS); // must timer
        write_reg(C4001_ARRAY_INDEX_0, (uint8_t *)START_SENSOR, strlen(START_SENSOR));
    } else if (mode == RECOVERSEN) {
        write_reg(C4001_ARRAY_INDEX_0, (uint8_t *)STOP_SENSOR, strlen(STOP_SENSOR));
        uapi_systick_delay_ms(C4001_DELAY_200_MS);
        write_reg(C4001_ARRAY_INDEX_0, (uint8_t *)RECOVER_SENSOR, strlen(RECOVER_SENSOR));
        uapi_systick_delay_ms(C4001_DELAY_800_MS); // must timer
        write_reg(C4001_ARRAY_INDEX_0, (uint8_t *)START_SENSOR, strlen(START_SENSOR));
        uapi_systick_delay_ms(C4001_DELAY_500_MS);
    }
}

bool set_sensor_mode(e_mode_t mode)
{
    sensor_stop();
    if (mode == EXITMODE) {
        write_reg(C4001_ARRAY_INDEX_0, (uint8_t *)EXIST_MODE, strlen(EXIST_MODE));
        uapi_systick_delay_ms(C4001_DELAY_50_MS);
    } else {
        write_reg(C4001_ARRAY_INDEX_0, (uint8_t *)SPEED_MODE, strlen(SPEED_MODE));
        uapi_systick_delay_ms(C4001_DELAY_50_MS);
    }
    uapi_systick_delay_ms(C4001_DELAY_50_MS);
    write_reg(C4001_ARRAY_INDEX_0, (uint8_t *)SAVE_CONFIG, strlen(SAVE_CONFIG));
    uapi_systick_delay_ms(C4001_DELAY_500_MS);
    write_reg(C4001_ARRAY_INDEX_0, (uint8_t *)START_SENSOR, strlen(START_SENSOR));
    uapi_systick_delay_ms(C4001_DELAY_100_MS);
    return true;
}

bool set_trig_sensitivity(uint8_t sensitivity)
{
    if (sensitivity > C4001_SENSITIVITY_MAX) {
        return false;
    }

    char data[] = "setSensitivity 255 1";                            // 分配在栈上，可写
    data[C4001_STR_POS_SENSITIVITY_TRIG_OFFSET] = sensitivity + '0'; // 修改有效
    write_cmd(data, data, (uint8_t)C4001_ARRAY_INDEX_1);
    return true;
}

uint8_t get_trig_sensitivity(void)
{
    s_response_data_t response_data;
    uint8_t temp[C4001_STATUS_BUFFER_SIZE] = {0};
    read_reg(C4001_ARRAY_INDEX_0, temp, C4001_STATUS_BUFFER_SIZE);
    char *data = "getSensitivity";
    response_data = write_read_cmd(data, (uint8_t)C4001_ARRAY_INDEX_1);
    if (response_data.status) {
        return response_data.response1;
    }
    return C4001_ARRAY_INDEX_0;
}

bool set_keep_sensitivity(uint8_t sensitivity)
{
    if (sensitivity > C4001_SENSITIVITY_MAX) {
        return false;
    }

    char data[] = "setSensitivity 1 255";
    data[C4001_STR_POS_SENSITIVITY_KEEP_OFFSET] = sensitivity + '0';
    write_cmd(data, data, (uint8_t)C4001_ARRAY_INDEX_1);
    return true;
}

uint8_t get_keep_sensitivity(void)
{
    s_response_data_t response_data;
    uint8_t temp[C4001_STATUS_BUFFER_SIZE] = {0};
    read_reg(C4001_ARRAY_INDEX_0, temp, C4001_STATUS_BUFFER_SIZE);
    char *data = "getSensitivity";
    response_data = write_read_cmd(data, (uint8_t)C4001_ARRAY_INDEX_1);
    if (response_data.status) {
        return response_data.response2;
    }
    return C4001_ARRAY_INDEX_0;
}

bool set_delay(uint8_t trig, uint16_t keep)
{
    if (trig > C4001_TRIG_DELAY_MAX) {
        return false;
    }
    if (keep < C4001_KEEP_DELAY_MIN || keep > C4001_KEEP_DELAY_MAX) {
        return false;
    }

    // trig: 百分比 -> 秒 (trig * 0.01)
    float trig_val = trig * C4001_TRIG_DELAY_TO_SEC_MULTIPLIER;
    // keep: 乘0.5
    float keep_val = keep * C4001_KEEP_DELAY_TO_SEC_MULTIPLIER;

    char cmd[C4001_CMD_BUFFER_SIZE] = {0};
    // 格式化命令字符串
    snprintf(cmd, sizeof(cmd), "setLatency %.1f %.1f", trig_val, keep_val);

    // 调用底层写命令函数
    write_cmd(cmd, cmd, C4001_ARRAY_INDEX_1);

    return true;
}

uint8_t get_trig_delay(void)
{
    s_response_data_t response_data;
    char *data = "getLatency";
    response_data = write_read_cmd(data, (uint8_t)C4001_ARRAY_INDEX_1);
    if (response_data.status) {
        return response_data.response1 * C4001_TRIG_DELAY_FROM_SEC_MULTIPLIER;
    }
    return C4001_ARRAY_INDEX_0;
}

uint16_t get_keep_timeout(void)
{
    s_response_data_t response_data;
    char *data = "getLatency";
    response_data = write_read_cmd(data, (uint8_t)C4001_ARRAY_INDEX_2);
    if (response_data.status) {
        return response_data.response2 * C4001_KEEP_DELAY_FROM_SEC_MULTIPLIER;
    }
    return C4001_ARRAY_INDEX_0;
}

bool set_detection_range(uint16_t min, uint16_t max, uint16_t trig)
{
    if (max < C4001_RANGE_MAX_MIN || max > C4001_RANGE_MAX_MAX) {
        return false;
    }
    if (min < C4001_RANGE_MIN_MIN || min > max) {
        return false;
    }

    float min_val = min / C4001_RANGE_TO_METER_DIVISOR;
    float max_val = max / C4001_RANGE_TO_METER_DIVISOR;
    float trig_val = trig / C4001_RANGE_TO_METER_DIVISOR;

    char data1[C4001_CMD_BUFFER_SIZE] = {0};
    char data2[C4001_CMD_BUFFER_SIZE] = {0};

    // 拼接命令
    snprintf(data1, sizeof(data1), "setRange %.2f %.2f", min_val, max_val);
    snprintf(data2, sizeof(data2), "setTrigRange %.2f", trig_val);

    // 发送命令
    write_cmd(data1, data2, C4001_ARRAY_INDEX_2);

    return true;
}

uint16_t get_trig_range(void)
{
    s_response_data_t response_data;
    char *data = "getTrigRange";
    response_data = write_read_cmd(data, (uint8_t)C4001_ARRAY_INDEX_1);
    if (response_data.status) {
        return response_data.response1 * C4001_RANGE_FROM_METER_MULTIPLIER;
    }
    return C4001_ARRAY_INDEX_0;
}

uint16_t get_max_range(void)
{
    s_response_data_t response_data;
    char *data = "getRange";
    response_data = write_read_cmd(data, (uint8_t)C4001_ARRAY_INDEX_2);
    if (response_data.status) {
        return response_data.response2 * C4001_RANGE_FROM_METER_MULTIPLIER;
    }
    return C4001_ARRAY_INDEX_0;
}

uint16_t get_min_range(void)
{
    s_response_data_t response_data;
    char *data = "getRange";
    response_data = write_read_cmd(data, (uint8_t)C4001_ARRAY_INDEX_2);
    if (response_data.status) {
        return response_data.response1 * C4001_RANGE_FROM_METER_MULTIPLIER;
    }
    return C4001_ARRAY_INDEX_0;
}

uint8_t get_target_number(void)
{
    static uint8_t flash_number = C4001_ARRAY_INDEX_0;
    uint8_t len = 0;
    uint8_t temp[C4001_STATUS_BUFFER_SIZE] = {0};
    s_all_data_t data;
    len = read_reg(C4001_ARRAY_INDEX_0, temp, C4001_STATUS_BUFFER_SIZE);
    data = analysis_data(temp, len);
    if (data.target.number != C4001_ARRAY_INDEX_0) {
        flash_number = C4001_ARRAY_INDEX_0;
        g_buffer.number = data.target.number;
        g_buffer.range = data.target.range / C4001_RANGE_TO_METER_DIVISOR;
        g_buffer.speed = data.target.speed / C4001_RANGE_TO_METER_DIVISOR;
        g_buffer.energy = data.target.energy;
    } else {
        g_buffer.number = C4001_ARRAY_INDEX_1;
        if (flash_number++ > C4001_FLASH_COUNTER_THRESHOLD) {
            g_buffer.number = C4001_ARRAY_INDEX_0;
            g_buffer.range = 0;
            g_buffer.speed = 0;
            g_buffer.energy = 0;
        }
    }
    return data.target.number;
}

float get_target_speed(void)
{
    return g_buffer.speed;
}

float get_target_range(void)
{
    return g_buffer.range;
}

uint32_t get_target_energy(void)
{
    return g_buffer.energy;
}

bool set_detect_thres(uint16_t min, uint16_t max, uint16_t thres)
{
    if (max > C4001_RANGE_MAX_LIMIT) {
        return false;
    }
    if (min > max) {
        return false;
    }

    float min_val = min / C4001_RANGE_TO_METER_DIVISOR;
    float max_val = max / C4001_RANGE_TO_METER_DIVISOR;

    char data1[C4001_CMD_BUFFER_SIZE] = {0};
    char data2[C4001_CMD_BUFFER_SIZE] = {0};

    // 拼接字符串
    snprintf(data1, sizeof(data1), "setRange %.2f %.2f", min_val, max_val);
    snprintf(data2, sizeof(data2), "setThrFactor %u", thres);

    // 发送命令
    write_cmd(data1, data2, C4001_ARRAY_INDEX_2);

    return true;
}

bool set_io_polarity(uint8_t value)
{
    if (value > C4001_IO_POLARITY_MAX) {
        return false;
    }

    char data[C4001_IO_BUFFER_SIZE] = {0};                 // 可写缓冲
    snprintf(data, sizeof(data), "setGpioMode %d", value); // 拼接字符串
    write_cmd(data, data, (uint8_t)C4001_ARRAY_INDEX_1);
    return true;
}

uint8_t get_io_polarity(void)
{
    s_response_data_t response_data;
    char *data = "getGpioMode 1";
    response_data = write_read_cmd(data, (uint8_t)C4001_ARRAY_INDEX_2);
    if (response_data.status) {
        return response_data.response2;
    }
    return C4001_ARRAY_INDEX_0;
}

bool set_pwm(uint8_t pwm1, uint8_t pwm2, uint8_t timer)
{
    if (pwm1 > C4001_PWM_MAX || pwm2 > C4001_PWM_MAX) {
        return false;
    }

    char data[C4001_CMD_BUFFER_SIZE] = {0};

    // 拼接成命令字符串，例如：set_pwm 50 75 10
    snprintf(data, sizeof(data), "setPwm %u %u %u", pwm1, pwm2, timer);

    // 发送命令
    write_cmd(data, data, C4001_ARRAY_INDEX_1);

    return true;
}

s_pwm_data_t get_pwm(void)
{
    s_pwm_data_t pwm_data;
    memset(&pwm_data, C4001_ARRAY_INDEX_0, sizeof(s_pwm_data_t));

    s_response_data_t response_data;
    char *data = "getPwm";
    response_data = write_read_cmd(data, (uint8_t)C4001_ARRAY_INDEX_3);
    if (response_data.status) {
        pwm_data.pwm1 = response_data.response1;
        pwm_data.pwm2 = response_data.response2;
        pwm_data.timer = response_data.response3;
    }
    return pwm_data;
}

uint16_t get_t_min_range(void)
{
    s_response_data_t response_data;
    char *data = "getRange";
    response_data = write_read_cmd(data, (uint8_t)C4001_ARRAY_INDEX_1);
    if (response_data.status) {
        return response_data.response1 * C4001_RANGE_FROM_METER_MULTIPLIER;
    }
    return C4001_ARRAY_INDEX_0;
}

uint16_t get_t_max_range(void)
{
    s_response_data_t response_data;
    char *data = "getRange";
    response_data = write_read_cmd(data, (uint8_t)C4001_ARRAY_INDEX_2);
    if (response_data.status) {
        return response_data.response2 * C4001_RANGE_FROM_METER_MULTIPLIER;
    }
    return C4001_ARRAY_INDEX_0;
}

uint16_t get_thres_range(void)
{
    s_response_data_t response_data;
    char *data = "getThrFactor";
    response_data = write_read_cmd(data, (uint8_t)C4001_ARRAY_INDEX_1);
    if (response_data.status) {
        return response_data.response1;
    }
    return C4001_ARRAY_INDEX_0;
}

void set_fretting_detection(e_switch_t sta)
{
    char data[C4001_IO_BUFFER_SIZE] = {0};
    snprintf(data, sizeof(data), "setMicroMotion %d", sta);
    write_cmd(data, data, (uint8_t)C4001_ARRAY_INDEX_1);
}

e_switch_t get_fretting_detection(void)
{
    s_response_data_t response_data;
    char *data = "getMicroMotion";
    response_data = write_read_cmd(data, (uint8_t)C4001_ARRAY_INDEX_1);
    if (response_data.status) {
        return (e_switch_t)response_data.response1;
    }
    return (e_switch_t)C4001_ARRAY_INDEX_0;
}
