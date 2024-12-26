#ifndef MQTT_DEMO_H
#define MQTT_DEMO_H

typedef enum {
    EN_MSG_PARS = 1,
    EN_MSG_REPORT = 2,
} en_msg_type_t;

typedef struct {
    int msg_type;
    char temp[10];
    char humi[10];
    char *receive_payload;
} MQTT_msg;

#endif