 /**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 *
 * Description: msg queue debug config. \n
 *
 * History: \n
 * 2024-6-21, Create file. \n
 */
#ifndef _DEBUG_
#define _DEBUG_

#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>

#define _PATH_ "."  // 当前路径下
#define _PROJ_ID_ 0x6668
#define _SIZE_ 3000

#define SERVER_TYPE 2
#define CLIENT_TYPE 1

struct msgbuffer {
    long mtype;          // 标识是谁发的消息，也可以标识是给谁发的消息，此处举例是前者
    char mtext[_SIZE_];  // 存放发的消息
};
int creat_queue();  // 由server创建消息队列，保证创建的是一个全新的消息队列
int mq_get_queue();  // client访问消息队列，若不存在，则创建它；若存在，则打开它（不能保证打开的是一个全新的消息队列）


int mq_send_msg(int msgqueue_id, int msg_src, char *msg);                 // 生产消息
int mq_recv_msg(int msgqueue_id, int msg_dst, char out[], int out_len);  // 消费消息

int delete_msgqueue(int msgqueue_id);  // 删除消息队列

#endif
