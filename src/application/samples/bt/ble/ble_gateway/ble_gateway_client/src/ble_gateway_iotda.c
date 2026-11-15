/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2026-2026.
 * Description: Huawei Cloud IoTDA MQTTS adapter for the BLE gateway sample.
 */

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "securec.h"
#include "soc_osal.h"
#ifndef IOT_LITEOS_ADAPT
#define IOT_LITEOS_ADAPT
#endif
#include "MQTTClient.h"
#include "cJSON.h"
#include "mbedtls/md.h"
#include "ble_gateway_client.h"
#include "ble_gateway_iotda.h"
#include "ble_gateway_iotda_ca.h"

#define IOTDA_LOG "[ble mqtt gateway]"
#define IOTDA_QOS 1
#define IOTDA_KEEP_ALIVE_S 60
#define IOTDA_TIMEOUT_MS 10000L
#define IOTDA_LOOP_DELAY_MS 100U
#define IOTDA_URI_MAX_LEN 320U
#define IOTDA_CLIENT_ID_MAX_LEN 320U
#define IOTDA_TOPIC_MAX_LEN 512U
#define IOTDA_PAYLOAD_MAX_LEN 512U
#define IOTDA_REQUEST_ID_MAX_LEN 128U
#define IOTDA_HMAC_SIZE 32U
#define IOTDA_PASSWORD_HEX_SIZE (IOTDA_HMAC_SIZE * 2U + 1U)
#define IOTDA_HEX_NIBBLE_BITS 4U
#define IOTDA_COMMAND_SUCCESS 0
#define IOTDA_COMMAND_FAILURE 1

typedef struct {
    bool node_state_ready;
    bool node_online;
    bool report_ready;
    ble_gateway_report_t report;
    bool command_waiting;
    bool response_ready;
    int response_code;
    char request_id[IOTDA_REQUEST_ID_MAX_LEN];
} iotda_bridge_state_t;

typedef struct {
    char uri[IOTDA_URI_MAX_LEN];
    char client_id[IOTDA_CLIENT_ID_MAX_LEN];
    char report_topic[IOTDA_TOPIC_MAX_LEN];
    char command_topic[IOTDA_TOPIC_MAX_LEN];
    char password[IOTDA_PASSWORD_HEX_SIZE];
    MQTTClient client;
} iotda_connection_context_t;

static osal_mutex g_iotda_mutex;
static bool g_iotda_initialized;
static volatile bool g_mqtt_connected;
static iotda_bridge_state_t g_bridge_state;

static bool iotda_is_safe_identifier(const char *value, bool allow_dot)
{
    size_t index;
    if (value == NULL || value[0] == '\0') {
        return false;
    }
    for (index = 0; value[index] != '\0'; index++) {
        char ch = value[index];
        if ((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z') || (ch >= '0' && ch <= '9') || ch == '-' ||
            ch == '_' || (allow_dot && ch == '.')) {
            continue;
        }
        return false;
    }
    return true;
}

static bool iotda_config_valid(void)
{
    return iotda_is_safe_identifier(CONFIG_IOTDA_MQTT_HOST, true) &&
           iotda_is_safe_identifier(CONFIG_IOTDA_DEVICE_ID, false) &&
           iotda_is_safe_identifier(CONFIG_IOTDA_SERVICE_ID, false) && strlen(CONFIG_IOTDA_DEVICE_SECRET) > 0U &&
           strlen(CONFIG_IOTDA_AUTH_TIMESTAMP) == 10U;
}

static bool iotda_build_password(char output[IOTDA_PASSWORD_HEX_SIZE])
{
    static const char hex[] = "0123456789abcdef";
    const mbedtls_md_info_t *info = mbedtls_md_info_from_type(MBEDTLS_MD_SHA256);
    unsigned char digest[IOTDA_HMAC_SIZE] = {0};
    size_t index;
    int ret;

    if (info == NULL) {
        return false;
    }
    /* IoTDA's published test vector uses the timestamp as HMAC key and DeviceSecret as the message. */
    ret = mbedtls_md_hmac(info, (const unsigned char *)CONFIG_IOTDA_AUTH_TIMESTAMP,
                          strlen(CONFIG_IOTDA_AUTH_TIMESTAMP), (const unsigned char *)CONFIG_IOTDA_DEVICE_SECRET,
                          strlen(CONFIG_IOTDA_DEVICE_SECRET), digest);
    if (ret != 0) {
        return false;
    }
    for (index = 0; index < IOTDA_HMAC_SIZE; index++) {
        output[index * 2U] = hex[digest[index] >> IOTDA_HEX_NIBBLE_BITS];
        output[index * 2U + 1U] = hex[digest[index] & 0x0FU];
    }
    output[IOTDA_PASSWORD_HEX_SIZE - 1U] = '\0';
    (void)memset_s(digest, sizeof(digest), 0, sizeof(digest));
    return true;
}

static bool iotda_queue_response(const char *request_id, int result_code)
{
    bool queued = false;
    if (osal_mutex_lock(&g_iotda_mutex) != OSAL_SUCCESS) {
        return false;
    }
    if ((!g_bridge_state.command_waiting || strcmp(g_bridge_state.request_id, request_id) == 0) &&
        !g_bridge_state.response_ready &&
        strcpy_s(g_bridge_state.request_id, sizeof(g_bridge_state.request_id), request_id) == EOK) {
        g_bridge_state.response_code = result_code;
        g_bridge_state.response_ready = true;
        g_bridge_state.command_waiting = false;
        queued = true;
    }
    osal_mutex_unlock(&g_iotda_mutex);
    return queued;
}

static bool iotda_copy_request_id(const char *topic, char output[IOTDA_REQUEST_ID_MAX_LEN])
{
    const char marker[] = "request_id=";
    const char *request_id = strstr(topic, marker);
    size_t length;
    if (request_id == NULL) {
        return false;
    }
    request_id += sizeof(marker) - 1U;
    length = strlen(request_id);
    if (length == 0U || length >= IOTDA_REQUEST_ID_MAX_LEN || !iotda_is_safe_identifier(request_id, false)) {
        return false;
    }
    return strcpy_s(output, IOTDA_REQUEST_ID_MAX_LEN, request_id) == EOK;
}

static bool iotda_parse_interval_command(const char *payload, int payload_len, uint32_t *interval_s)
{
    char *json_text;
    cJSON *root = NULL;
    cJSON *service_id;
    cJSON *command_name;
    cJSON *paras;
    cJSON *interval;
    bool valid = false;

    if (payload == NULL || payload_len <= 0 || interval_s == NULL) {
        return false;
    }
    json_text = osal_kmalloc((size_t)payload_len + 1U, OSAL_GFP_KERNEL);
    if (json_text == NULL) {
        return false;
    }
    if (memcpy_s(json_text, (size_t)payload_len + 1U, payload, (size_t)payload_len) != EOK) {
        osal_kfree(json_text);
        return false;
    }
    json_text[payload_len] = '\0';
    root = cJSON_Parse(json_text);
    if (root != NULL) {
        service_id = cJSON_GetObjectItemCaseSensitive(root, "service_id");
        command_name = cJSON_GetObjectItemCaseSensitive(root, "command_name");
        paras = cJSON_GetObjectItemCaseSensitive(root, "paras");
        interval = cJSON_IsObject(paras) ? cJSON_GetObjectItemCaseSensitive(paras, "interval_s") : NULL;
        if (cJSON_IsString(service_id) && cJSON_IsString(command_name) && cJSON_IsNumber(interval) &&
            strcmp(service_id->valuestring, CONFIG_IOTDA_SERVICE_ID) == 0 &&
            strcmp(command_name->valuestring, "set_sample_interval") == 0 &&
            interval->valuedouble == (double)interval->valueint &&
            interval->valueint >= (int)BLE_GATEWAY_MIN_INTERVAL_S &&
            interval->valueint <= (int)BLE_GATEWAY_MAX_INTERVAL_S) {
            *interval_s = (uint32_t)interval->valueint;
            valid = true;
        }
    }
    cJSON_Delete(root);
    (void)memset_s(json_text, (size_t)payload_len + 1U, 0, (size_t)payload_len + 1U);
    osal_kfree(json_text);
    return valid;
}

static int iotda_message_arrived(void *context, char *topic_name, int topic_len, MQTTClient_message *message)
{
    char request_id[IOTDA_REQUEST_ID_MAX_LEN] = {0};
    uint32_t interval_s = 0;
    errcode_t ret = ERRCODE_FAIL;
    bool busy = false;
    (void)context;
    (void)topic_len;

    if (topic_name != NULL && message != NULL && iotda_copy_request_id(topic_name, request_id) &&
        iotda_parse_interval_command((const char *)message->payload, message->payloadlen, &interval_s)) {
        if (osal_mutex_lock(&g_iotda_mutex) == OSAL_SUCCESS) {
            busy = g_bridge_state.command_waiting || g_bridge_state.response_ready;
            if (!busy && strcpy_s(g_bridge_state.request_id, sizeof(g_bridge_state.request_id), request_id) == EOK) {
                g_bridge_state.command_waiting = true;
            }
            osal_mutex_unlock(&g_iotda_mutex);
        } else {
            busy = true;
        }
        if (!busy) {
            ret = ble_gateway_client_set_sample_interval(interval_s);
            osal_printk("%s cloud command accepted: interval=%u s\r\n", IOTDA_LOG, interval_s);
        }
    }
    if (ret != ERRCODE_SUCC && request_id[0] != '\0') {
        if (iotda_queue_response(request_id, IOTDA_COMMAND_FAILURE)) {
            osal_printk("%s cloud command rejected or BLE node unavailable\r\n", IOTDA_LOG);
        } else {
            osal_printk("%s cloud command ignored while another command is pending\r\n", IOTDA_LOG);
        }
    }
    MQTTClient_freeMessage(&message);
    MQTTClient_free(topic_name);
    return 1;
}

static void iotda_connection_lost(void *context, char *cause)
{
    (void)context;
    (void)cause;
    g_mqtt_connected = false;
    osal_printk("%s MQTTS connection lost\r\n", IOTDA_LOG);
}

static int iotda_publish(MQTTClient client, const char *topic, const char *payload)
{
    MQTTClient_message message = MQTTClient_message_initializer;
    MQTTClient_deliveryToken token;
    int ret;

    message.payload = (void *)payload;
    message.payloadlen = (int)strlen(payload);
    message.qos = IOTDA_QOS;
    message.retained = 0;
    ret = MQTTClient_publishMessage(client, topic, &message, &token);
    if (ret == MQTTCLIENT_SUCCESS) {
        ret = MQTTClient_waitForCompletion(client, token, IOTDA_TIMEOUT_MS);
    }
    return ret;
}

static bool iotda_take_report(ble_gateway_report_t *report)
{
    bool ready = false;
    if (osal_mutex_lock(&g_iotda_mutex) == OSAL_SUCCESS) {
        if (g_bridge_state.report_ready) {
            *report = g_bridge_state.report;
            g_bridge_state.report_ready = false;
            ready = true;
        }
        osal_mutex_unlock(&g_iotda_mutex);
    }
    return ready;
}

static bool iotda_take_node_state(bool *online)
{
    bool ready = false;
    if (osal_mutex_lock(&g_iotda_mutex) == OSAL_SUCCESS) {
        if (g_bridge_state.node_state_ready) {
            *online = g_bridge_state.node_online;
            g_bridge_state.node_state_ready = false;
            ready = true;
        }
        osal_mutex_unlock(&g_iotda_mutex);
    }
    return ready;
}

static void iotda_restore_report_if_empty(const ble_gateway_report_t *report)
{
    if (osal_mutex_lock(&g_iotda_mutex) == OSAL_SUCCESS) {
        if (!g_bridge_state.report_ready) {
            g_bridge_state.report = *report;
            g_bridge_state.report_ready = true;
        }
        osal_mutex_unlock(&g_iotda_mutex);
    }
}

static void iotda_restore_node_state_if_empty(bool online)
{
    if (osal_mutex_lock(&g_iotda_mutex) == OSAL_SUCCESS) {
        if (!g_bridge_state.node_state_ready) {
            g_bridge_state.node_online = online;
            g_bridge_state.node_state_ready = true;
        }
        osal_mutex_unlock(&g_iotda_mutex);
    }
}

static bool iotda_take_response(char request_id[IOTDA_REQUEST_ID_MAX_LEN], int *result_code)
{
    bool ready = false;
    if (osal_mutex_lock(&g_iotda_mutex) == OSAL_SUCCESS) {
        if (g_bridge_state.response_ready &&
            strcpy_s(request_id, IOTDA_REQUEST_ID_MAX_LEN, g_bridge_state.request_id) == EOK) {
            *result_code = g_bridge_state.response_code;
            g_bridge_state.response_ready = false;
            ready = true;
        }
        osal_mutex_unlock(&g_iotda_mutex);
    }
    return ready;
}

static int iotda_publish_report(MQTTClient client, const char *topic, const ble_gateway_report_t *report)
{
    char payload[IOTDA_PAYLOAD_MAX_LEN] = {0};
    uint16_t temperature_abs = (uint16_t)(report->temperature_tenths_celsius < 0 ?
        -report->temperature_tenths_celsius : report->temperature_tenths_celsius);
    int length = snprintf_s(payload, sizeof(payload), sizeof(payload) - 1U,
        "{\"services\":[{\"service_id\":\"%s\",\"properties\":{"
        "\"node_online\":true,\"node_id\":%u,\"temperature\":%s%u.%u,\"humidity\":%u.%u,"
        "\"pressure\":%u,\"seq\":%u}}]}",
        CONFIG_IOTDA_SERVICE_ID, report->node_id, report->temperature_tenths_celsius < 0 ? "-" : "",
        temperature_abs / 10U, temperature_abs % 10U, report->humidity_tenths_percent / 10U,
        report->humidity_tenths_percent % 10U, report->pressure_pa, report->sequence);
    if (length < 0) {
        return MQTTCLIENT_FAILURE;
    }
    return iotda_publish(client, topic, payload);
}

static int iotda_publish_node_state(MQTTClient client, const char *topic, bool online)
{
    char payload[IOTDA_PAYLOAD_MAX_LEN] = {0};
    if (snprintf_s(payload, sizeof(payload), sizeof(payload) - 1U,
                   "{\"services\":[{\"service_id\":\"%s\",\"properties\":{"
                   "\"node_online\":%s,\"node_id\":%u}}]}",
                   CONFIG_IOTDA_SERVICE_ID, online ? "true" : "false", BLE_GATEWAY_NODE_ID_DEFAULT) < 0) {
        return MQTTCLIENT_FAILURE;
    }
    return iotda_publish(client, topic, payload);
}

static int iotda_publish_response(MQTTClient client, const char *request_id, int result_code)
{
    char topic[IOTDA_TOPIC_MAX_LEN] = {0};
    char payload[IOTDA_PAYLOAD_MAX_LEN] = {0};
    const char *result = result_code == 0 ? "success" : "failed";
    if (snprintf_s(topic, sizeof(topic), sizeof(topic) - 1U,
                   "$oc/devices/%s/sys/commands/response/request_id=%s", CONFIG_IOTDA_DEVICE_ID, request_id) < 0 ||
        snprintf_s(payload, sizeof(payload), sizeof(payload) - 1U,
                   "{\"result_code\":%d,\"response_name\":\"COMMAND_RESPONSE\","
                   "\"paras\":{\"result\":\"%s\"}}",
                   result_code, result) < 0) {
        return MQTTCLIENT_FAILURE;
    }
    return iotda_publish(client, topic, payload);
}

void ble_gateway_iotda_init(void)
{
    MQTTClient_init_options mqtt_init_options = MQTTClient_init_options_initializer;
    if (g_iotda_initialized) {
        return;
    }
    if (osal_mutex_init(&g_iotda_mutex) == OSAL_SUCCESS) {
        MQTTClient_global_init(&mqtt_init_options);
        (void)memset_s(&g_bridge_state, sizeof(g_bridge_state), 0, sizeof(g_bridge_state));
        g_iotda_initialized = true;
    }
}

void ble_gateway_iotda_enqueue_report(const ble_gateway_report_t *report)
{
    if (!g_iotda_initialized || report == NULL || osal_mutex_lock(&g_iotda_mutex) != OSAL_SUCCESS) {
        return;
    }
    g_bridge_state.report = *report;
    g_bridge_state.report_ready = true;
    osal_mutex_unlock(&g_iotda_mutex);
}

void ble_gateway_iotda_command_result(bool success)
{
    if (!g_iotda_initialized || osal_mutex_lock(&g_iotda_mutex) != OSAL_SUCCESS) {
        return;
    }
    if (g_bridge_state.command_waiting) {
        g_bridge_state.response_code = success ? IOTDA_COMMAND_SUCCESS : IOTDA_COMMAND_FAILURE;
        g_bridge_state.response_ready = true;
        g_bridge_state.command_waiting = false;
    }
    osal_mutex_unlock(&g_iotda_mutex);
}

void ble_gateway_iotda_node_state(bool online)
{
    if (!g_iotda_initialized || osal_mutex_lock(&g_iotda_mutex) != OSAL_SUCCESS) {
        return;
    }
    g_bridge_state.node_online = online;
    g_bridge_state.node_state_ready = true;
    osal_mutex_unlock(&g_iotda_mutex);
}

static bool iotda_prepare_connection(iotda_connection_context_t *context)
{
    if (!iotda_config_valid() || !iotda_build_password(context->password)) {
        return false;
    }
    if (snprintf_s(context->uri, sizeof(context->uri), sizeof(context->uri) - 1U, "ssl://%s:%d",
                   CONFIG_IOTDA_MQTT_HOST, CONFIG_IOTDA_MQTT_PORT) < 0 ||
        snprintf_s(context->client_id, sizeof(context->client_id), sizeof(context->client_id) - 1U, "%s_0_0_%s",
                   CONFIG_IOTDA_DEVICE_ID, CONFIG_IOTDA_AUTH_TIMESTAMP) < 0 ||
        snprintf_s(context->report_topic, sizeof(context->report_topic), sizeof(context->report_topic) - 1U,
                   "$oc/devices/%s/sys/properties/report", CONFIG_IOTDA_DEVICE_ID) < 0 ||
        snprintf_s(context->command_topic, sizeof(context->command_topic), sizeof(context->command_topic) - 1U,
                   "$oc/devices/%s/sys/commands/#", CONFIG_IOTDA_DEVICE_ID) < 0) {
        osal_printk("%s IoTDA connection parameter is too long\r\n", IOTDA_LOG);
        return false;
    }
    return true;
}

static bool iotda_connect(iotda_connection_context_t *context)
{
    MQTTClient_SSLOptions ssl_options = MQTTClient_SSLOptions_initializer;
    MQTTClient_connectOptions connect_options = MQTTClient_connectOptions_initializer;
    cert_string trust_store = {BLE_GATEWAY_IOTDA_CA, BLE_GATEWAY_IOTDA_CA_SIZE};
    int ret;

    ret = MQTTClient_create(&context->client, context->uri, context->client_id, MQTTCLIENT_PERSISTENCE_NONE, NULL);
    if (ret != MQTTCLIENT_SUCCESS) {
        return false;
    }
    ret = MQTTClient_setCallbacks(context->client, NULL, iotda_connection_lost, iotda_message_arrived, NULL);
    if (ret != MQTTCLIENT_SUCCESS) {
        MQTTClient_destroy(&context->client);
        return false;
    }
    ssl_options.los_trustStore = &trust_store;
    ssl_options.sslVersion = MQTT_SSL_VERSION_TLS_1_2;
    ssl_options.enableServerCertAuth = 1;
    connect_options.keepAliveInterval = IOTDA_KEEP_ALIVE_S;
    connect_options.cleansession = 1;
    connect_options.username = CONFIG_IOTDA_DEVICE_ID;
    connect_options.password = context->password;
    connect_options.ssl = &ssl_options;
    connect_options.MQTTVersion = MQTTVERSION_3_1_1;
    ret = MQTTClient_connect(context->client, &connect_options);
    (void)memset_s(context->password, sizeof(context->password), 0, sizeof(context->password));
    if (ret != MQTTCLIENT_SUCCESS) {
        osal_printk("%s MQTTS connect failed: %d\r\n", IOTDA_LOG, ret);
        MQTTClient_destroy(&context->client);
        return false;
    }
    ret = MQTTClient_subscribe(context->client, context->command_topic, IOTDA_QOS);
    if (ret != MQTTCLIENT_SUCCESS) {
        osal_printk("%s command subscription failed: %d\r\n", IOTDA_LOG, ret);
        (void)MQTTClient_disconnect(context->client, IOTDA_TIMEOUT_MS);
        MQTTClient_destroy(&context->client);
        return false;
    }
    return true;
}

static bool iotda_publish_pending_node_state(const iotda_connection_context_t *context)
{
    bool node_online = false;
    int ret;
    if (!iotda_take_node_state(&node_online)) {
        return true;
    }
    ret = iotda_publish_node_state(context->client, context->report_topic, node_online);
    osal_printk("%s BLE node state %s\r\n", IOTDA_LOG,
                ret == MQTTCLIENT_SUCCESS ? "reported" : "report failed");
    if (ret != MQTTCLIENT_SUCCESS) {
        iotda_restore_node_state_if_empty(node_online);
        return false;
    }
    return true;
}

static bool iotda_publish_pending_report(const iotda_connection_context_t *context)
{
    ble_gateway_report_t report = {0};
    int ret;
    if (!iotda_take_report(&report)) {
        return true;
    }
    ret = iotda_publish_report(context->client, context->report_topic, &report);
    osal_printk("%s property report %s: node=%u seq=%u\r\n", IOTDA_LOG,
                ret == MQTTCLIENT_SUCCESS ? "sent" : "failed", report.node_id, report.sequence);
    if (ret != MQTTCLIENT_SUCCESS) {
        iotda_restore_report_if_empty(&report);
        return false;
    }
    return true;
}

static bool iotda_publish_pending_response(const iotda_connection_context_t *context)
{
    char request_id[IOTDA_REQUEST_ID_MAX_LEN] = {0};
    int result_code = IOTDA_COMMAND_FAILURE;
    int ret;
    if (!iotda_take_response(request_id, &result_code)) {
        return true;
    }
    ret = iotda_publish_response(context->client, request_id, result_code);
    osal_printk("%s command response %s\r\n", IOTDA_LOG, ret == MQTTCLIENT_SUCCESS ? "sent" : "failed");
    if (ret != MQTTCLIENT_SUCCESS) {
        (void)iotda_queue_response(request_id, result_code);
        return false;
    }
    return true;
}

static void iotda_process_messages(const iotda_connection_context_t *context)
{
    while (g_mqtt_connected && MQTTClient_isConnected(context->client)) {
        if (!iotda_publish_pending_node_state(context) || !iotda_publish_pending_report(context) ||
            !iotda_publish_pending_response(context)) {
            break;
        }
        osal_msleep(IOTDA_LOOP_DELAY_MS);
    }
}

void ble_gateway_iotda_run(void)
{
    iotda_connection_context_t context = {0};

    ble_gateway_iotda_init();
    if (!g_iotda_initialized || !iotda_prepare_connection(&context)) {
        osal_printk("%s private Kconfig is incomplete or invalid\r\n", IOTDA_LOG);
        (void)memset_s(context.password, sizeof(context.password), 0, sizeof(context.password));
        return;
    }
    if (!iotda_connect(&context)) {
        (void)memset_s(context.password, sizeof(context.password), 0, sizeof(context.password));
        return;
    }
    g_mqtt_connected = true;
    osal_printk("%s IoTDA MQTTS online, QoS 1\r\n", IOTDA_LOG);
    ble_gateway_client_restart_scan();
    iotda_process_messages(&context);
    g_mqtt_connected = false;
    (void)MQTTClient_disconnect(context.client, IOTDA_TIMEOUT_MS);
    MQTTClient_destroy(&context.client);
}
