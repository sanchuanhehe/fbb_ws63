/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2024. All rights reserved.
 *
 * @if Eng
 * @brief Declares data types and interfaces for the SLE sensor report server.
 * @else
 * @brief 声明 SLE 传感器上报服务端的数据类型与接口。
 * @endif
 *
 * History: \n
 * 2024-06-01, Create file. \n
 */

#ifndef SLE_SENSOR_REPORT_SERVER_H
#define SLE_SENSOR_REPORT_SERVER_H

#include <stdint.h>
#include "errcode.h"

/* SSAP UUID definitions. / SSAP UUID 定义。 */
#define SENSOR_SERVICE_UUID              0x5555
#define SENSOR_DATA_PROPERTY_UUID        0x5656   /* Periodic data property. / 常规数据属性。 */
#define SENSOR_ALARM_PROPERTY_UUID       0x5757   /* Alarm data property. / 告警数据属性。 */

/* Advertising name. / 广播名称。 */
#define SENSOR_SERVER_NAME               "sensor_server"

/* Timer period in milliseconds. / 定时器周期，单位为毫秒。 */
#define SENSOR_REPORT_INTERVAL_MS        1000

/* Alarm thresholds scaled by 100. / 放大 100 倍的告警阈值。 */
#define TEMP_ALARM_HIGH                  8000   /* 80.00C */
#define TEMP_ALARM_LOW                   (-1000) /* -10.00C */
#define HUMIDITY_ALARM_LOW               20     /* 20% */

/* Property permissions. / 属性权限。 */
#define SENSOR_PROPERTY_PERMISSIONS      (SSAP_PERMISSION_READ)

/* Periodic data property: read and notify. / 常规数据属性：读和通知。 */
#define SENSOR_DATA_PROPERTY_OP_INDICATION \
    (SSAP_OPERATE_INDICATION_BIT_READ | \
     SSAP_OPERATE_INDICATION_BIT_NOTIFY)

#define SENSOR_DATA_PROPERTY_PERMISSIONS  (SSAP_PERMISSION_READ)

/* Alarm property: read and indicate with a CCCD. / 告警属性：读和指示，需要 CCCD。 */
#define SENSOR_ALARM_PROPERTY_OP_INDICATION \
    (SSAP_OPERATE_INDICATION_BIT_READ | \
     SSAP_OPERATE_INDICATION_BIT_INDICATE)

/* Sensor data frame type. / 传感器数据帧类型。 */
#define SENSOR_FRAME_TYPE_PERIODIC       0x01
#define SENSOR_FRAME_TYPE_ALARM          0x02

/**
 * @if Eng
 * @brief Defines a data type used by this sample.
 * @else
 * @brief 定义本案例使用的数据类型。
 * @endif
 */
typedef struct {
    uint8_t  frame_type;
    uint8_t  sensor_count;
    uint32_t timestamp;
    int16_t  temperature;
    uint8_t  humidity;
    uint16_t light;
} __attribute__((packed)) sensor_data_frame_t;

/* Public API */
/**
 * @if Eng
 * @brief Initializes the feature implemented by \c sle_sensor_report_server_init.
 * @else
 * @brief 初始化 \c sle_sensor_report_server_init 对应的功能。
 * @endif
 */
errcode_t sle_sensor_report_server_init(void);
/**
 * @if Eng
 * @brief Reports whether the SLE link is connected.
 * @else
 * @brief 返回 SLE 链路是否已连接。
 * @endif
 */
uint16_t sle_sensor_report_server_is_connected(void);

#endif /* SLE_SENSOR_REPORT_SERVER_H */
