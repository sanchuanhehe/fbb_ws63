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
const char *get_city_name(const char *city_id)
{
    if (strcmp(city_id, "CH280401") == 0)
        return "Meizhou";
    if (strcmp(city_id, "CH280501") == 0)
        return "Shantou";
    if (strcmp(city_id, "CH280601") == 0)
        return "Shenzhen";
    if (strcmp(city_id, "CH280701") == 0)
        return "Zhuhai";
    return "Unknown city";
}

/* 根据天气编码输出天气描述 */
const char *get_weather_description(const char *weather_level)
{
    if (strcmp(weather_level, "00") == 0)
        return "Sunny";
    if (strcmp(weather_level, "01") == 0)
        return "Cloudy";
    if (strcmp(weather_level, "02") == 0)
        return "Overcast";
    if (strcmp(weather_level, "03") == 0)
        return "Shower";
    if (strcmp(weather_level, "04") == 0)
        return "Thundershower";
    if (strcmp(weather_level, "05") == 0)
        return "Thundershower with hail";
    if (strcmp(weather_level, "06") == 0)
        return "Sleet";
    if (strcmp(weather_level, "07") == 0)
        return "Light rain";
    if (strcmp(weather_level, "08") == 0)
        return "Moderate rain";
    if (strcmp(weather_level, "09") == 0)
        return "Heavy rain";
    if (strcmp(weather_level, "10") == 0)
        return "Storm";
    if (strcmp(weather_level, "11") == 0)
        return "Heavy storm";
    if (strcmp(weather_level, "12") == 0)
        return "Severe storm";
    if (strcmp(weather_level, "13") == 0)
        return "Snow flurry";
    if (strcmp(weather_level, "14") == 0)
        return "Light snow";
    if (strcmp(weather_level, "15") == 0)
        return "Moderate snow";
    if (strcmp(weather_level, "16") == 0)
        return "Heavy snow";
    if (strcmp(weather_level, "17") == 0)
        return "Snowstorm";
    if (strcmp(weather_level, "18") == 0)
        return "Fog";
    if (strcmp(weather_level, "19") == 0)
        return "Ice rain";
    if (strcmp(weather_level, "20") == 0)
        return "Duststorm";
    if (strcmp(weather_level, "21") == 0)
        return "Light to moderate rain";
    if (strcmp(weather_level, "22") == 0)
        return "Moderate to heavy rain";
    if (strcmp(weather_level, "23") == 0)
        return "Heavy rain to storm";
    if (strcmp(weather_level, "24") == 0)
        return "Storm to heavy storm";
    if (strcmp(weather_level, "25") == 0)
        return "Heavy to severe storm";
    if (strcmp(weather_level, "26") == 0)
        return "Light to moderate snow";
    if (strcmp(weather_level, "27") == 0)
        return "Moderate to heavy snow";
    if (strcmp(weather_level, "28") == 0)
        return "Heavy snow to snowstorm";
    if (strcmp(weather_level, "29") == 0)
        return "Dust";
    if (strcmp(weather_level, "30") == 0)
        return "Sand";
    if (strcmp(weather_level, "31") == 0)
        return "Sandstorm";
    if (strcmp(weather_level, "32") == 0)
        return "Dense fog";
    if (strcmp(weather_level, "49") == 0)
        return "Heavy dense fog";
    if (strcmp(weather_level, "53") == 0)
        return "Haze";
    if (strcmp(weather_level, "54") == 0)
        return "Moderate haze";
    if (strcmp(weather_level, "55") == 0)
        return "Severe haze";
    if (strcmp(weather_level, "56") == 0)
        return "Hazardous haze";
    if (strcmp(weather_level, "57") == 0)
        return "Heavy fog";
    if (strcmp(weather_level, "58") == 0)
        return "Extra-heavy dense fog";
    if (strcmp(weather_level, "99") == 0)
        return "Unknown";
    if (strcmp(weather_level, "100") == 0)
        return "Windy";
    return "Invalid code";
}

/* 根据风力等级编码输出风力描述 */
const char *get_wind_description(const char *wind_scale)
{
    if (strcmp(wind_scale, "0") == 0)
        return "Light breeze";
    if (strcmp(wind_scale, "1") == 0)
        return "3-4 level";
    if (strcmp(wind_scale, "2") == 0)
        return "4-5 level";
    if (strcmp(wind_scale, "3") == 0)
        return "5-6 level";
    if (strcmp(wind_scale, "4") == 0)
        return "6-7 level";
    if (strcmp(wind_scale, "5") == 0)
        return "7-8 level";
    if (strcmp(wind_scale, "6") == 0)
        return "8-9 level";
    if (strcmp(wind_scale, "7") == 0)
        return "9-10 level";
    if (strcmp(wind_scale, "8") == 0)
        return "10-11 level";
    if (strcmp(wind_scale, "9") == 0)
        return "11-12 level";
    return "Invalid code";
}

/* 根据风向编码输出风向描述 */
const char *get_wind_direction_description(const char *wind_direction)
{
    if (strcmp(wind_direction, "0") == 0)
        return "No sustained wind direction";
    if (strcmp(wind_direction, "1") == 0)
        return "Northeast wind";
    if (strcmp(wind_direction, "2") == 0)
        return "East wind";
    if (strcmp(wind_direction, "3") == 0)
        return "Southeast wind";
    if (strcmp(wind_direction, "4") == 0)
        return "South wind";
    if (strcmp(wind_direction, "5") == 0)
        return "Southwest wind";
    if (strcmp(wind_direction, "6") == 0)
        return "West wind";
    if (strcmp(wind_direction, "7") == 0)
        return "Northwest wind";
    if (strcmp(wind_direction, "8") == 0)
        return "North wind";
    if (strcmp(wind_direction, "9") == 0)
        return "Variable wind";
    return "Invalid code";
}

/* 解析 JSON 数据并输出天气、风力、风向、城市名称、时间和相对湿度 */
void parse_weather_data(const char *json_data)
{
    // 解析城市ID
    const char *city_id = strstr(json_data, "\"cityId\":\"");
    if (city_id)
    {
        city_id += 10;                          // 跳过 "city_id":""
        const char *end = strchr(city_id, '"'); // 找到结束引号
        if (end)
        {
            char city_id[50];
            strncpy(city_id, city_id, end - city_id);
            city_id[end - city_id] = '\0'; // 添加字符串结束符
            const char *city_name = get_city_name(city_id);
            osal_printk("City: %s\r\n", city_name);
        }
    }

    // 解析时间
    const char *time = strstr(json_data, "\"lastUpdate\":\"");
    if (time)
    {
        time += 14;                          // 跳过 "time":""
        const char *end = strchr(time, '"'); // 找到结束引号
        if (end)
        {
            char last_update[50];
            strncpy(last_update, time, end - time);
            last_update[end - time] = '\0'; // 添加字符串结束符
            osal_printk("Time: %s\r\n", last_update);
        }
    }

    // 解析相对湿度
    const char *humidity = strstr(json_data, "\"sd\":\"");
    if (humidity)
    {
        humidity += 6;                           // 跳过 "humidity":""
        const char *end = strchr(humidity, '"'); // 找到结束引号
        if (end)
        {
            char humidity[10];
            strncpy(humidity, humidity, end - humidity);
            humidity[end - humidity] = '\0'; // 添加字符串结束符
            osal_printk("humidity: %s\r\n", humidity);
        }
    }

    // 解析温度
    const char *temperature = strstr(json_data, "\"qw\":\"");
    if (temperature)
    {
        temperature += 6;                           // 跳过 "temperature":""
        const char *end = strchr(temperature, '"'); // 找到结束引号
        if (end)
        {
            char temperature[10];
            strncpy(temperature, temperature, end - temperature);
            temperature[end - temperature] = '\0'; // 添加字符串结束符
            osal_printk("temperature: %s\r\n", temperature);
        }
    }

    // 解析天气编码
    const char *weather_level = strstr(json_data, "\"numtq\":\"");
    if (weather_level)
    {
        weather_level += 9;                           // 跳过 "weather_level":""
        const char *end = strchr(weather_level, '"'); // 找到结束引号
        if (end)
        {
            char weather_level_value[10];
            strncpy(weather_level_value, weather_level, end - weather_level);
            weather_level_value[end - weather_level] = '\0'; // 添加字符串结束符
            const char *weather_desc = get_weather_description(weather_level_value);
            osal_printk("Weather: %s\r\n", weather_desc);
        }
    }

    // 解析风力等级编码
    const char *wind_scale = strstr(json_data, "\"numfl\":\"");
    if (wind_scale)
    {
        wind_scale += 9;                           // 跳过 "wind_scale":""
        const char *end = strchr(wind_scale, '"'); // 找到结束引号
        if (end)
        {
            char wind_scale_value[10];
            strncpy(wind_scale_value, wind_scale, end - wind_scale);
            wind_scale_value[end - wind_scale] = '\0'; // 添加字符串结束符
            const char *wind_desc = get_wind_description(wind_scale_value);
            osal_printk("Wind level: %s\r\n", wind_desc);
        }
    }

    // 解析风向编码
    const char *wind_direction = strstr(json_data, "\"numfx\":\"");
    if (wind_direction)
    {
        wind_direction += 9;                           // 跳过 "wind_direction":""
        const char *end = strchr(wind_direction, '"'); // 找到结束引号
        if (end)
        {
            char wind_direction_value[10];
            strncpy(wind_direction_value, wind_direction, end - wind_direction);
            wind_direction_value[end - wind_direction] = '\0'; // 添加字符串结束符
            const char *wind_direction_desc = get_wind_direction_description(wind_direction_value);
            osal_printk("Wind direction: %s\r\n", wind_direction_desc);
        }
    }
}