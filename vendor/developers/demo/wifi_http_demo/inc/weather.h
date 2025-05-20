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
#ifndef WEATHER_H
#define WEATHER_H

#include <stdio.h>
#include <string.h>

#define CITY_ID_LENGTH 10
#define TIME_LENGTH 14
#define HUMIDITY_LENGTH 6
#define TEMPERATURE_LENGTH 6
#define WEATHER_LEVEL_LENGTH 9
#define WIND_SCALE_LENGTH 9
#define WIND_DIRECTION_LENGTH 9

const char *get_city_name(const char *cityId);
const char *get_weather_description(const char *numtq);
const char *get_wind_description(const char *numfl);
const char *get_wind_direction_description(const char *numfx);
void parse_weather_data(const char *json_data);

#endif // WEATHER_H