/**
 * Copyright (c) Adragon
 *
 * Description: WiFi STA and HTTP Get to get weather forecasts. \n
 *              This file is used to parse data in json format.
 *              
 *
 * History: \n
 * 2025-03-18, Create file. \n
 */
#include "weather.h"
#include "osal_debug.h"  
/* 根据城市ID输出城市英文名称 */
const char* get_city_name(const char* cityId) {
    if (strcmp(cityId, "CH280401") == 0) return "Meizhou";
    if (strcmp(cityId, "CH280501") == 0) return "Shantou";
    if (strcmp(cityId, "CH280601") == 0) return "Shenzhen";
    if (strcmp(cityId, "CH280701") == 0) return "Zhuhai";
    return "Unknown city";
}

/* 根据天气编码输出天气描述 */
const char* get_weather_description(const char* numtq) {
    if (strcmp(numtq, "00") == 0) return "Sunny";
    if (strcmp(numtq, "01") == 0) return "Cloudy";
    if (strcmp(numtq, "02") == 0) return "Overcast";
    if (strcmp(numtq, "03") == 0) return "Shower";
    if (strcmp(numtq, "04") == 0) return "Thundershower";
    if (strcmp(numtq, "05") == 0) return "Thundershower with hail";
    if (strcmp(numtq, "06") == 0) return "Sleet";
    if (strcmp(numtq, "07") == 0) return "Light rain";
    if (strcmp(numtq, "08") == 0) return "Moderate rain";
    if (strcmp(numtq, "09") == 0) return "Heavy rain";
    if (strcmp(numtq, "10") == 0) return "Storm";
    if (strcmp(numtq, "11") == 0) return "Heavy storm";
    if (strcmp(numtq, "12") == 0) return "Severe storm";
    if (strcmp(numtq, "13") == 0) return "Snow flurry";
    if (strcmp(numtq, "14") == 0) return "Light snow";
    if (strcmp(numtq, "15") == 0) return "Moderate snow";
    if (strcmp(numtq, "16") == 0) return "Heavy snow";
    if (strcmp(numtq, "17") == 0) return "Snowstorm";
    if (strcmp(numtq, "18") == 0) return "Fog";
    if (strcmp(numtq, "19") == 0) return "Ice rain";
    if (strcmp(numtq, "20") == 0) return "Duststorm";
    if (strcmp(numtq, "21") == 0) return "Light to moderate rain";
    if (strcmp(numtq, "22") == 0) return "Moderate to heavy rain";
    if (strcmp(numtq, "23") == 0) return "Heavy rain to storm";
    if (strcmp(numtq, "24") == 0) return "Storm to heavy storm";
    if (strcmp(numtq, "25") == 0) return "Heavy to severe storm";
    if (strcmp(numtq, "26") == 0) return "Light to moderate snow";
    if (strcmp(numtq, "27") == 0) return "Moderate to heavy snow";
    if (strcmp(numtq, "28") == 0) return "Heavy snow to snowstorm";
    if (strcmp(numtq, "29") == 0) return "Dust";
    if (strcmp(numtq, "30") == 0) return "Sand";
    if (strcmp(numtq, "31") == 0) return "Sandstorm";
    if (strcmp(numtq, "32") == 0) return "Dense fog";
    if (strcmp(numtq, "49") == 0) return "Heavy dense fog";
    if (strcmp(numtq, "53") == 0) return "Haze";
    if (strcmp(numtq, "54") == 0) return "Moderate haze";
    if (strcmp(numtq, "55") == 0) return "Severe haze";
    if (strcmp(numtq, "56") == 0) return "Hazardous haze";
    if (strcmp(numtq, "57") == 0) return "Heavy fog";
    if (strcmp(numtq, "58") == 0) return "Extra-heavy dense fog";
    if (strcmp(numtq, "99") == 0) return "Unknown";
    if (strcmp(numtq, "100") == 0) return "Windy";
    return "Invalid code";
}

/* 根据风力等级编码输出风力描述 */
const char* get_wind_description(const char* numfl) {
    if (strcmp(numfl, "0") == 0) return "Light breeze";
    if (strcmp(numfl, "1") == 0) return "3-4 level";
    if (strcmp(numfl, "2") == 0) return "4-5 level";
    if (strcmp(numfl, "3") == 0) return "5-6 level";
    if (strcmp(numfl, "4") == 0) return "6-7 level";
    if (strcmp(numfl, "5") == 0) return "7-8 level";
    if (strcmp(numfl, "6") == 0) return "8-9 level";
    if (strcmp(numfl, "7") == 0) return "9-10 level";
    if (strcmp(numfl, "8") == 0) return "10-11 level";
    if (strcmp(numfl, "9") == 0) return "11-12 level";
    return "Invalid code";
}

/* 根据风向编码输出风向描述 */
const char* get_wind_direction_description(const char* numfx) {
    if (strcmp(numfx, "0") == 0) return "No sustained wind direction";
    if (strcmp(numfx, "1") == 0) return "Northeast wind";
    if (strcmp(numfx, "2") == 0) return "East wind";
    if (strcmp(numfx, "3") == 0) return "Southeast wind";
    if (strcmp(numfx, "4") == 0) return "South wind";
    if (strcmp(numfx, "5") == 0) return "Southwest wind";
    if (strcmp(numfx, "6") == 0) return "West wind";
    if (strcmp(numfx, "7") == 0) return "Northwest wind";
    if (strcmp(numfx, "8") == 0) return "North wind";
    if (strcmp(numfx, "9") == 0) return "Variable wind";
    return "Invalid code";
}

/* 解析 JSON 数据并输出天气、风力、风向、城市名称、时间和相对湿度 */
void parse_weather_data(const char* json_data) {
    // 解析城市ID
    const char* cityId = strstr(json_data, "\"cityId\":\"");
    if (cityId) {
        cityId += 10; // 跳过 "cityId":""
        const char* end = strchr(cityId, '"'); // 找到结束引号
        if (end) {
            char city_id[50];
            strncpy(city_id, cityId, end - cityId);
            city_id[end - cityId] = '\0'; // 添加字符串结束符
            const char* city_name = get_city_name(city_id);
            osal_printk("City: %s\r\n", city_name);
        }
    }

    // 解析时间
    const char* lastUpdate = strstr(json_data, "\"lastUpdate\":\"");
    if (lastUpdate) {
        lastUpdate += 14; // 跳过 "lastUpdate":""
        const char* end = strchr(lastUpdate, '"'); // 找到结束引号
        if (end) {
            char last_update[50];
            strncpy(last_update, lastUpdate, end - lastUpdate);
            last_update[end - lastUpdate] = '\0'; // 添加字符串结束符
            osal_printk("Time: %s\r\n", last_update);
        }
    }

    // 解析相对湿度
    const char* sd = strstr(json_data, "\"sd\":\"");
    if (sd) {
        sd += 6; // 跳过 "sd":""
        const char* end = strchr(sd, '"'); // 找到结束引号
        if (end) {
            char humidity[10];
            strncpy(humidity, sd, end - sd);
            humidity[end - sd] = '\0'; // 添加字符串结束符
            osal_printk("sd: %s\r\n", humidity);
        }
    }

    // 解析温度
    const char* qw = strstr(json_data, "\"qw\":\"");
    if (qw) {
        qw += 6; // 跳过 "qw":""
        const char* end = strchr(qw, '"'); // 找到结束引号
        if (end) {
            char temperature[10];
            strncpy(temperature, qw, end - qw);
            temperature[end - qw] = '\0'; // 添加字符串结束符
            osal_printk("qw: %s\r\n", temperature);
        }
    }

    // 解析天气编码
    const char* numtq = strstr(json_data, "\"numtq\":\"");
    if (numtq) {
        numtq += 9; // 跳过 "numtq":""
        const char* end = strchr(numtq, '"'); // 找到结束引号
        if (end) {
            char numtq_value[10];
            strncpy(numtq_value, numtq, end - numtq);
            numtq_value[end - numtq] = '\0'; // 添加字符串结束符
            const char* weather_desc = get_weather_description(numtq_value);
            osal_printk("Weather: %s\r\n", weather_desc);
        }
    }

    // 解析风力等级编码
    const char* numfl = strstr(json_data, "\"numfl\":\"");
    if (numfl) {
        numfl += 9; // 跳过 "numfl":""
        const char* end = strchr(numfl, '"'); // 找到结束引号
        if (end) {
            char numfl_value[10];
            strncpy(numfl_value, numfl, end - numfl);
            numfl_value[end - numfl] = '\0'; // 添加字符串结束符
            const char* wind_desc = get_wind_description(numfl_value);
            osal_printk("Wind level: %s\r\n", wind_desc);
        }
    }

    // 解析风向编码
    const char* numfx = strstr(json_data, "\"numfx\":\"");
    if (numfx) {
        numfx += 9; // 跳过 "numfx":""
        const char* end = strchr(numfx, '"'); // 找到结束引号
        if (end) {
            char numfx_value[10];
            strncpy(numfx_value, numfx, end - numfx);
            numfx_value[end - numfx] = '\0'; // 添加字符串结束符
            const char* wind_direction_desc = get_wind_direction_description(numfx_value);
            osal_printk("Wind direction: %s\r\n", wind_direction_desc);
        }
    }
}