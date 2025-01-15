#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include "cJSON.h"

int string_length(char *str)
{
    if (str == NULL) {
        return 0;
    }
    int len = 0;
    char *temp_str = str;
    while (*temp_str++ != '\0') {
        len++;
    }
    return len;
}

char *combine_strings(int str_amount, char *str1, ...)
{
    int length = string_length(str1) + 1;
    if (length == 1) {
        return NULL; // 如果第一个字符串为空
    }

    char *result = malloc(length);
    if (result == NULL) {
        return NULL; // 内存分配失败
    }

    strcpy(result, str1); // 复制第一个字符串

    va_list args;
    va_start(args, str1);

    char *tem_str;
    while (--str_amount > 0) {
        tem_str = va_arg(args, char *);
        if (tem_str == NULL) {
            continue; // 跳过空字符串
        }
        length += string_length(tem_str);
        result = realloc(result, length);
        if (result == NULL) {
            return NULL; // 内存重新分配失败
        }
        strcat(result, tem_str); // 拼接字符串
    }
    va_end(args);

    return result; // 返回拼接后的字符串
}

char *make_json(char *service_id, char *temperature, char *humidity)
{
    // 创建 JSON 对象
    cJSON *root = cJSON_CreateObject();
    // 创建 services 数组
    cJSON *services = cJSON_CreateArray();
    // 创建 service 对象
    cJSON *service = cJSON_CreateObject();
    cJSON_AddStringToObject(service, "service_id", service_id);

    // 创建 properties 对象
    cJSON *properties = cJSON_CreateObject();
    cJSON_AddStringToObject(properties, "temperature", temperature);
    cJSON_AddStringToObject(properties, "humidity", humidity);

    // 将 properties 添加到 service
    cJSON_AddItemToObject(service, "properties", properties);

    // 将 service 添加到 services 数组
    cJSON_AddItemToArray(services, service);

    // 将 services 添加到 root
    cJSON_AddItemToObject(root, "services", services);

    // 打印 JSON 字符串
    char *json_string = cJSON_Print(root);
    // 释放内存
    cJSON_Delete(root);
    return json_string;
}

char *parse_json(char *json_string)
{
    char *string = NULL;
    // 解析 JSON 字符串
    cJSON *root = cJSON_Parse(json_string);
    if (root == NULL) {
        printf("Error parsing JSON\n");
        return NULL;
    }
    // 获取 paras 对象中的 beep 项
    cJSON *paras = cJSON_GetObjectItem(root, "paras");
    cJSON *beep = cJSON_GetObjectItem(paras, "beep");
    // 检查并输出 beep 的值
    if (beep && cJSON_IsString(beep)) {
        printf("beep: %s\n", beep->valuestring);
        string = beep->valuestring;
    } else {
        printf("beep not found or is not a string\n");
    }
    // 释放内存
    cJSON_Delete(root);
    return string;
}