/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
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
#include <stdio.h>
#include <unistd.h>
#include "cmsis_os2.h"
#include "soc_osal.h"
#include "pinctrl.h"
#include "i2c.h"

#include "nfc_config.h"

#define OFFSET_0 0
#define OFFSET_1 1
#define OFFSET_2 2
#define OFFSET_3 3
#define OFFSET_7 7
#define BITS_PER_CHAR 8
#define SER_BUM_LEN 9
#define PROTOCOL_LEN 4
#define I2C_ADDR_LEN 2
#define TX_FIFO_LEN 32
#define FIFO_LEN 24
#define CRC_LEN 2

#define DELAY_10_MS 10000
#define DELAY_1_MS 1000

t4t_file g_current_file;

unsigned char g_capability_container[15] = {
    0x00, 0x0F, // CCLEN
    0x20,       // Mapping Version
    0x00, 0xF6, // MLe 必须是F6  写成FF超过256字节就会分帧  但是写成F6就不会分帧
    0x00, 0xF6, // MLc 必须是F6  写成FF超过256字节就会分帧  但是写成F6就不会分帧
    0x04,       // NDEF消息格式 05的话就是私有
    0x06,       // NDEF消息长度
    0xE1, 0x04, // NDEF FILE ID       NDEF的文件标识符
    0x03, 0x84, // NDEF最大长度
    0x00,       // Read Access 可读
    0x00        // Write Access 可写
};

#define HUAWEI (0)
#define TAOBAO (1)
#define WECHAT (2)
#define NFC_TAG_NAME WECHAT
unsigned char g_ndef_file[1024] = {
#if (NFC_TAG_NAME == HUAWEI)
    /* http://wwww.huawei.com */
    0x00,
    0x0F, // PAYLOAD LENGTH
    // PAYLOAD LENGTH BYTES OF PAYLOAD
    0xD1, 0x01, 0x0B, 0x55, 0x01, 0x68, 0x75, 0x61, 0x77, 0x65, 0x69, 0x2E, 0x63, 0x6F, 0x6D, // huawei.com
#elif (NFC_TAG_NAME == TAOBAO)
    0x00,
    0x23, // PAYLOAD LENGTH
    // PAYLOAD LENGTH BYTES OF PAYLOAD
    0xd4, 0x0f, 0x11, 0x61, 0x6e, 0x64, 0x72, 0x6f, 0x69, 0x64, 0x2e, 0x63, 0x6f, 0x6d, 0x3a, // android.com:
    0x70, 0x6b, 0x67, 0x63, 0x6f, 0x6d, 0x2e,                                                 // pkgcom.
    0x74, 0x61, 0x6f, 0x62, 0x61, 0x6f, 0x2e,                                                 // taobao.
    0x74, 0x61, 0x6f, 0x62, 0x61, 0x6f,                                                       // taobao
#else
    /* wechat */
    0x00,
    0x20, // PAYLOAD LENGTH
    // PAYLOAD LENGTH BYTES OF PAYLOAD
    0xd4, 0x0f, 0x0e, 0x61, 0x6e, 0x64, 0x72, 0x6f, 0x69, 0x64, 0x2e, 0x63, 0x6f, 0x6d, 0x3a, // android.com:
    0x70, 0x6b, 0x67, 0x63, 0x6f, 0x6d, 0x2e,                                                 // pkgcom.
    0x74, 0x65, 0x6e, 0x63, 0x65, 0x6e, 0x74, 0x2e, 0x6d, 0x6d,                               // tencent.mm
#endif
};
unsigned char g_fm327_fifo[1024];
unsigned char g_irq_data_in = 0; // 非接数据接收终端标识
unsigned char g_irq_txdone = 0;
unsigned char g_rf_len;
unsigned char g_irq_rxdone = 0;
unsigned char g_irq_data_wl = 0;
unsigned char g_flag_first_frame = 0; // 卡片首帧标识

unsigned char g_fm365_sak_config =
    0x20; // 只有在通道模式下才需要配置，14443-3通道配置成0x00,14443-4通道配置成0x20.目前路由器走的是14443-4通道
unsigned char g_protocol_4[4];   // -4 通道
unsigned char g_protocol_3[4];   // -3 通道
unsigned char g_protocol_tag[4]; // tag模式
unsigned char g_tag_mode = 0;    // 定义走Tag芯片，Tag_mode和AFE_mode 不能同时为1
unsigned char g_afe_mode = 1;    // 定义走通道芯片，Tag_mode和AFE_mode 不能同时为1
unsigned char g_nfc_mode = 0;    // mode为0时，芯片为通道模式，mode为1时，芯片为Tag模式

/* i2c read */
unsigned int write_read(unsigned char reg_high_8bit_cmd,
                        unsigned char reg_low_8bit_cmd,
                        unsigned char *recv_data,
                        unsigned char send_len,
                        unsigned char read_len)
{
    uint32_t id = CONFIG_I2C_BUS_ID;
    i2c_data_t co8i_nfc_i2c_read_data = {0};
    i2c_data_t c081nfc_i2c_write_cmd_addr = {0};

    unsigned char send_user_cmd[2] = {reg_high_8bit_cmd, reg_low_8bit_cmd};

    memset(recv_data, 0x0, sizeof(recv_data));
    memset(&co8i_nfc_i2c_read_data, 0x0, sizeof(i2c_data_t));

    c081nfc_i2c_write_cmd_addr.send_buf = send_user_cmd;
    c081nfc_i2c_write_cmd_addr.send_len = send_len;

    co8i_nfc_i2c_read_data.receive_buf = recv_data;
    co8i_nfc_i2c_read_data.receive_len = read_len;

    uapi_i2c_master_write(id, C081_NFC_ADDR & 0xFF, &c081nfc_i2c_write_cmd_addr);
    uapi_i2c_master_read(id, C081_NFC_ADDR | I2C_RD, &co8i_nfc_i2c_read_data);

    return 0;
}

void nfc_init(void)
{
    g_nfc_mode = 1;
    protocol_config();
    fm11_init();
}

void protocol_config(void)
{
    protocol_init_typedef protocol_init_structure;
    if (g_nfc_mode == 0) {
        g_afe_mode = 1;
        g_tag_mode = 0;
    } else {
        g_tag_mode = 1;
        g_afe_mode = 0;
    }

    if (g_afe_mode) {                     // 配置成通道模式
        if (g_fm365_sak_config == 0x20) { /* 14443-4通道模式 */
            protocol_init_structure.user_cfg0 = AFE_VOUTENSABLE;
            protocol_init_structure.user_cfg1 = AFE_INVENTORYENABLE_14443_4_RFONDISABLE;
            protocol_init_structure.user_cfg2 = AFE_TAG_WIP_ARBITRATECT;
            protocol_init_structure.user_check =
                ~(protocol_init_structure.user_cfg0 ^ protocol_init_structure.user_cfg1 ^
                  protocol_init_structure.user_cfg2);
            g_protocol_4[OFFSET_0] = protocol_init_structure.user_cfg0;
            g_protocol_4[OFFSET_1] = protocol_init_structure.user_cfg1;
            g_protocol_4[OFFSET_2] = protocol_init_structure.user_cfg2;
            g_protocol_4[OFFSET_3] = protocol_init_structure.user_check;
        } else if (g_fm365_sak_config == 0x00) { /* 14443-3通道模式 */
            protocol_init_structure.user_cfg0 = AFE_VOUTENSABLE;
            protocol_init_structure.user_cfg1 =
                AFE_INVENTORYENABLE_14443_3_RFONENABLE; // RFON要打开，要不初始化来不及做
            protocol_init_structure.user_cfg2 = AFE_TAG_WIP_ARBITRATECT;
            protocol_init_structure.user_check =
                ~(protocol_init_structure.user_cfg0 ^ protocol_init_structure.user_cfg1 ^
                  protocol_init_structure.user_cfg2);
            g_protocol_3[OFFSET_0] = protocol_init_structure.user_cfg0;
            g_protocol_3[OFFSET_1] = protocol_init_structure.user_cfg1;
            g_protocol_3[OFFSET_2] = protocol_init_structure.user_cfg2;
            g_protocol_3[OFFSET_3] = protocol_init_structure.user_check;
        }
    } else if (g_tag_mode) { /* tag SD模式 */
        protocol_init_structure.user_cfg0 = TAG_VOUTENABLE_OD_FD;
        protocol_init_structure.user_cfg1 = TAG_INVENTORYENABLE_AFE_INVENTORYENABLE_14443_4_RFONENSABLE;
        protocol_init_structure.user_cfg2 = TAG_SD_ARBITRATENONE;
        protocol_init_structure.user_check = ~(protocol_init_structure.user_cfg0 ^ protocol_init_structure.user_cfg1 ^
                                               protocol_init_structure.user_cfg2);
        g_protocol_tag[OFFSET_0] = protocol_init_structure.user_cfg0;
        g_protocol_tag[OFFSET_1] = protocol_init_structure.user_cfg1;
        g_protocol_tag[OFFSET_2] = protocol_init_structure.user_cfg2;
        g_protocol_tag[OFFSET_3] = protocol_init_structure.user_check;
    }
}
// 修改SAK必须要计算CRC8,这个是CRC8的计算函数
unsigned char crc8(unsigned char *data, unsigned char data_length)
{
    int i = 0;
    int j = 0;
    int crc8_value = 0xff;
    for (i = 0; i < data_length; i++) {
        crc8_value ^= data[i];
        for (j = 0; j < BITS_PER_CHAR; j++) {
            if ((crc8_value & 0x01) == 0x01) {
                crc8_value = (crc8_value >> 1) ^ 0xb8;
            } else {
                crc8_value >>= 1;
            }
            crc8_value &= 0xff;
        }
    }
    return crc8_value & 0xff;
}
// 计算并更新 CRC8 值
static void update_crc8(unsigned char *serial_number, unsigned char *atqa_sak)
{
    unsigned char crc8_raw_data[13] = {0x00};
    unsigned char crc8_data = 0x00;
    unsigned char data2[1] = {0x00};

    memcpy(crc8_raw_data, serial_number, SER_BUM_LEN);
    memcpy(&crc8_raw_data[SER_BUM_LEN], atqa_sak, PROTOCOL_LEN);

    crc8_data = crc8(crc8_raw_data, SER_BUM_LEN + PROTOCOL_LEN);

    fm11_write_eep(FM441_CRC8_EEADDRESS, 1, &crc8_data);
    fm11_read_eep(data2, FM441_CRC8_EEADDRESS, 1);
    printf("FM11_Init: read      data2[0x%02X]\n", data2[0]);
}

// 处理通道模式
static void handle_afe_mode(unsigned char *serial_number, unsigned char *atqa_sak)
{
    unsigned char status2;
    unsigned char status3;
    unsigned char sak2_4[1] = {0x20}; // 14443-4  通道模式
    unsigned char sak2_3[1] = {0x00}; // 14443-3  通道模式

    printf("FM11_Init: AFE_mode[1]\n");
    if (g_fm365_sak_config == 0x20) { // 14443-4  通道模式
        printf("FM11_Init: fm365SakConfig[0x20]\n");
        fm11_write_eep(FM441_SAK_SAK_CONTROL_EEADDRESS, 1, &sak2_4[0]); // 修改SAK，SAK一旦修改，必须修改CRC8
        fm11_write_eep(FM441_PROTOCOL_CONTROL_EEADDRESS, PROTOCOL_LEN, g_protocol_4);
    }
    if (g_fm365_sak_config == 0x00) { // 14443-3  通道模式
        printf("FM11_Init: fm365SakConfig[0x00]\n");
        fm11_write_eep(FM441_SAK_SAK_CONTROL_EEADDRESS, 1, &sak2_3[0]); // 修改SAK，SAK一旦修改，必须修改CRC8
        fm11_write_eep(FM441_PROTOCOL_CONTROL_EEADDRESS, PROTOCOL_LEN, g_protocol_3);
    }

    status2 =
        fm11_read_eep(serial_number, FM441_SERIAL_NUMBER_EEADDRESS, SER_BUM_LEN); // 读取serial_number ,为计算CRC8做准备
    status3 = fm11_read_eep(atqa_sak, FM441_ATQA_EEADDRESS, PROTOCOL_LEN);        // 读取ATQA_SAK ,为计算CRC8做准备
    if ((status2 != 1) && (status3 != 1)) {
        printf("FM11_Init[A]: read serial_number[0x%02X][0x%02X][0x%02X][0x%02X][...]\n", serial_number[OFFSET_0],
               serial_number[OFFSET_1], serial_number[OFFSET_2], serial_number[OFFSET_3]);
        printf("FM11_Init[A]: read      ATQA_SAK[0x%02X][0x%02X][0x%02X][0x%02X]\n", atqa_sak[OFFSET_0],
               atqa_sak[OFFSET_1], atqa_sak[OFFSET_2], atqa_sak[OFFSET_3]);
        update_crc8(serial_number, atqa_sak);
    }

    fm11_write_reg(FM441_RF_TXCTL_REG, 0x77); // 让切换模式字立马生效
    fm11_write_reg(RESET_SILENCE, 0x55);      // 让切换模式字立马生效
    osal_udelay(DELAY_1_MS);                  // 很重要，复位时间
}

// 处理 Tag 模式
static void handle_tag_mode(unsigned char *serial_number, unsigned char *atqa_sak)
{
    unsigned char status2;
    unsigned char status3;
    unsigned char sak2_3[1] = {0x00}; // 14443-3  通道模式
    unsigned char rbuf[16] = {0x00};  // 读取测试buf

    printf("FM11_Init: Tag_mode[1]\n");
    fm11_write_eep(FM441_SAK_SAK_CONTROL_EEADDRESS, 1, &sak2_3[0]); // 修改SAK，SAK一旦修改，必须修改CRC8
    fm11_write_eep(FM441_PROTOCOL_CONTROL_EEADDRESS, PROTOCOL_LEN, g_protocol_tag);

    status2 =
        fm11_read_eep(serial_number, FM441_SERIAL_NUMBER_EEADDRESS, SER_BUM_LEN); // 读取serial_number ,为计算CRC8做准备
    status3 = fm11_read_eep(atqa_sak, FM441_ATQA_EEADDRESS, PROTOCOL_LEN);        // 读取ATQA_SAK ,为计算CRC8做准备
    if ((status2 != 1) && (status3 != 1)) {
        printf("FM11_Init[T]: read serial_number[0x%02X][0x%02X][0x%02X][0x%02X][...]\n", serial_number[OFFSET_0],
               serial_number[OFFSET_1], serial_number[OFFSET_2], serial_number[OFFSET_3]);
        printf("FM11_Init[T]: read      ATQA_SAK[0x%02X][0x%02X][0x%02X][0x%02X]\n", atqa_sak[OFFSET_0],
               atqa_sak[OFFSET_1], atqa_sak[OFFSET_2], atqa_sak[OFFSET_3]);
        update_crc8(serial_number, atqa_sak);
    }

    fm11_write_reg(FM441_RF_TXCTL_REG, 0x77); // 让切换模式字立马生效
    fm11_write_reg(RESET_SILENCE, 0x55);      // 让切换模式字立马生效
    osal_udelay(DELAY_1_MS);                  // 很重要，复位时间

    fm11_read_eep(rbuf, FM441_PROTOCOL_CONTROL_EEADDRESS, PROTOCOL_LEN); // 测试下写成功没有
    fm11_read_eep(rbuf, FM441_SAK_SAK_CONTROL_EEADDRESS, 1);             // 测试下写成功没有
}

void fm11_init(void)
{
    unsigned char serial_number[9] = {0x00};
    unsigned char atqa_sak[4] = {0x00};

    fm11_read_eep(serial_number, FM441_SERIAL_NUMBER_EEADDRESS, SER_BUM_LEN);
    if (serial_number[OFFSET_7] == 0x10) {
        // FM11NT082C
        // UID[6]为0x10，FM11NC08和FM11NT081DI为0x00，如果想做到一套代码兼容FM11NT081和FM11NT082的话这段代码是关键
        if (g_afe_mode) {
            handle_afe_mode(serial_number, atqa_sak);
        } else if (g_tag_mode) {
            handle_tag_mode(serial_number, atqa_sak);
        }
    }
}
/* co8i 写命令: 该接口写eeprom 更改芯片配置 */
unsigned int c08i_nfc_i2c_write(unsigned char reg_high_8bit_cmd,
                                unsigned char reg_low_8bit_cmd,
                                unsigned char *data_buff,
                                unsigned char len)
{
    uint32_t id = CONFIG_I2C_BUS_ID;
    i2c_data_t c081nfc_i2c_write_cmd_addr = {0};
    unsigned char send_user_cmd[64] = {reg_high_8bit_cmd, reg_low_8bit_cmd};

    c081nfc_i2c_write_cmd_addr.send_buf = send_user_cmd;
    c081nfc_i2c_write_cmd_addr.send_len = I2C_ADDR_LEN + len;
    for (int i = 0; i < len; i++) {
        send_user_cmd[I2C_ADDR_LEN + i] = *(data_buff + i);
    }
    int ret = uapi_i2c_master_write(id, C081_NFC_ADDR & 0xFF, &c081nfc_i2c_write_cmd_addr);
    printf("----- c08i_nfc_i2c_write: write %d: %s\n", len, (ret == 0) ? "OK" : "NG");

    return 0;
}

/* 写寄存器 */
unsigned int write_fifo_reg(unsigned char reg_high_8bit_cmd, unsigned char reg_low_8bit_cmd, unsigned char data_buff)
{
    uint32_t id = CONFIG_I2C_BUS_ID;
    i2c_data_t c081nfc_i2c_write_cmd_addr = {0};
    unsigned char send_user_cmd[3] = {reg_high_8bit_cmd, reg_low_8bit_cmd, data_buff};

    c081nfc_i2c_write_cmd_addr.send_buf = send_user_cmd;
    c081nfc_i2c_write_cmd_addr.send_len = sizeof(send_user_cmd);

    int ret = uapi_i2c_master_write(id, C081_NFC_ADDR & 0xFF, &c081nfc_i2c_write_cmd_addr);

    return ret;
}

/* 写fifo data */
unsigned int write_fifo_data(unsigned char *data_buff, unsigned char len)
{
    uint32_t id = CONFIG_I2C_BUS_ID;
    i2c_data_t c081nfc_i2c_write_cmd_addr = {0};
    unsigned char send_user_cmd[128] = {0};

    memset(send_user_cmd, 0x0, sizeof(send_user_cmd));

    send_user_cmd[0] = 0xff;
    send_user_cmd[1] = 0xf0;

    for (int i = 0; i < len; i++) {
        send_user_cmd[I2C_ADDR_LEN + i] = *(data_buff + i);
    }
    c081nfc_i2c_write_cmd_addr.send_buf = send_user_cmd;
    c081nfc_i2c_write_cmd_addr.send_len = I2C_ADDR_LEN + len;
    int ret = uapi_i2c_master_write(id, C081_NFC_ADDR & 0xFF, &c081nfc_i2c_write_cmd_addr);

    return ret;
}

/* EEPROM page write */
void eep_write_page(unsigned char *buffer, unsigned short write_addr, unsigned char datalen)
{
    printf("----- eep_write_page :: WriteAddr[0x%02X],len[%d] -----\n", write_addr, datalen);
    c08i_nfc_i2c_write((unsigned char)((write_addr & 0xFF00) >> 0x8), (unsigned char)(write_addr & 0x00FF), buffer,
                       datalen);
    osal_udelay(DELAY_10_MS); // 必须延时10ms
}

/* 写EEPROM */
void fm11_write_eep(unsigned short addr, unsigned int len, unsigned char *wbuf)
{
    unsigned char offset;

    if (addr < FM11_E2_USER_ADDR || addr >= FM11_E2_MANUF_ADDR) {
        return;
    }
    if (addr % FM11_E2_BLOCK_SIZE) {
        offset = FM11_E2_BLOCK_SIZE - (addr % FM11_E2_BLOCK_SIZE);
        if (len > offset) {
            eep_write_page(wbuf, addr, offset);

            addr += offset;
            wbuf += offset;
            len -= offset;
        } else {
            eep_write_page(wbuf, addr, len);
            len = 0;
        }
    }
    while (len) {
        if (len >= FM11_E2_BLOCK_SIZE) {
            eep_write_page(wbuf, addr, FM11_E2_BLOCK_SIZE);
            addr += FM11_E2_BLOCK_SIZE;
            wbuf += FM11_E2_BLOCK_SIZE;
            len -= FM11_E2_BLOCK_SIZE;
        } else {
            eep_write_page(wbuf, addr, len);
            len = 0;
        }
    }
}

/* 读EEPROM */
unsigned int fm11_read_eep(unsigned char *data_buff, unsigned short read_addr, unsigned short len)
{
    write_read((unsigned char)((read_addr & 0xFF00) >> 0x08), (unsigned char)(read_addr & 0x00FF), data_buff, 0x02,
               len);

    return 0;
}
/* 读NFC寄存器 */
unsigned char fm11_read_reg(unsigned short addr)
{
    unsigned char pdata[10] = {0};
    unsigned char a = 0;

    if (fm11_read_eep(pdata, addr, 1) == 0) {
        a = pdata[0];
        return a;
    } else {
        printf("fm11_read_eep failed \r\n");
        return -1;
    }
}
/* 写NFC寄存器 */
unsigned char fm11_write_reg(unsigned short addr, unsigned char data)
{
    unsigned int status = 0;

    status = write_fifo_reg((unsigned char)((addr & 0xFF00) >> 0x08), (unsigned char)(addr & 0x00FF), data);
    if (status != 0) {
        return -1;
    }
    return 0;
}
/* 读取FIFO */
unsigned char fm11_read_fifo(unsigned char num_byte_to_read, unsigned char *pbuf)
{
    unsigned char read_fifo_len = num_byte_to_read;

    if (fm11_read_eep(pbuf, FM327_FIFO, read_fifo_len) == 0) {
        return 0;
    } else {
        return -1;
    }
}
/* 写FIFO */
unsigned char fm11_write_fifo(unsigned char *pbuf, unsigned char len)
{
    unsigned char status = 0;

    if (pbuf == NULL) {
        return -1;
    }
    status = write_fifo_data(pbuf, len);
    if (status != 0) {
        return -1;
    }
    return 0;
}

// 提取处理长度小于等于 TX_FIFO_LEN 的逻辑到单独函数
static void handle_small_length(unsigned char *sbuf, unsigned int slen)
{
    fm11_write_fifo(sbuf, slen); // write fifo	有多少发多少
    slen = 0;
    fm11_write_reg(RF_TXEN_REG, 0x55); // 写0x55时触发非接触口回发数据
}

// 提取处理长度大于 TX_FIFO_LEN 的逻辑到单独函数
static void handle_large_length(unsigned char *sbuf, unsigned int slen)
{
    fm11_write_fifo(sbuf, TX_FIFO_LEN); // write fifo    先发32字节进fifo
    fm11_write_reg(RF_TXEN_REG, 0x55);  // 写0x55时触发非接触口回发数据
    slen = slen - TX_FIFO_LEN;          // 待发长度－32
    sbuf = sbuf + TX_FIFO_LEN;          // 待发数据指针+32

    while (slen > 0) {
        if ((fm11_read_reg(FIFO_WORDCNT_REG) & 0x3F) <= 0x08) {
            if (slen <= FIFO_LEN) {
                fm11_write_fifo(sbuf, slen); // write fifo	先发32字节进fifo
                slen = 0;
            } else {
                fm11_write_fifo(sbuf, FIFO_LEN); // write fifo	先发32字节进fifo
                slen = slen - FIFO_LEN;          // 待发长度－24
                sbuf = sbuf + FIFO_LEN;          // 待发数据指针+24
            }
        }
    }
}

/* 数据回发 */
void fm11_data_send(unsigned int ilen, unsigned char *ibuf)
{
    unsigned int slen = 0;
    unsigned char *sbuf = NULL;

    if (ibuf == NULL) {
        return;
    }
    slen = ilen;
    sbuf = &ibuf[0];

    if (slen <= TX_FIFO_LEN) {
        handle_small_length(sbuf, slen);
    } else {
        handle_large_length(sbuf, slen);
    }
    g_irq_txdone = 0;
}

// 提取中断处理逻辑到单独函数
static void handle_interrupts(unsigned char *irq,
                              unsigned char *ret,
                              unsigned char *irq_data_wl,
                              unsigned char *irq_data_in)
{
    *irq_data_wl = 0;
    *irq = fm11_read_reg(MAIN_IRQ); // 查询中断标志

    if (*irq & MAIN_IRQ_FIFO) {
        *ret = fm11_read_reg(FIFO_IRQ);
        if (*ret & FIFO_IRQ_WL) {
            *irq_data_wl = 1;
        }
    }
    if (*irq & MAIN_IRQ_AUX) {
        fm11_read_reg(AUX_IRQ);
        fm11_write_reg(FIFO_FLUSH, 0xFF);
    }
    if (*irq & MAIN_IRQ_RX_START) {
        *irq_data_in = 1;
    }
}

// 提取 FIFO 读取逻辑到单独函数
static void handle_fifo_read(unsigned char *rbuf, unsigned int *rlen, unsigned int temp)
{
    fm11_read_fifo(temp, &rbuf[*rlen]);
    *rlen += temp;
}

/* 读取RF数据 */
/* 读取RF数据 */
unsigned int fm11_data_recv(unsigned char *rbuf)
{
    unsigned char irq = 0;
    unsigned char ret = 0;
    unsigned char g_irq_data_wl = 0;
    unsigned char g_irq_data_in = 0;
    unsigned int rlen = 0;
    unsigned int temp = 0;

#ifdef CHECK
    /* 查询方式 */
    while (1) {
        handle_interrupts(&irq, &ret, &g_irq_data_wl, &g_irq_data_in);

        if (g_irq_data_in && g_irq_data_wl) {
            g_irq_data_wl = 0;
            handle_fifo_read(rbuf, &rlen, 24); // 渐满之后读取24字节
        }
        if (irq & MAIN_IRQ_RX_DONE) {
            temp = (unsigned int)(fm11_read_reg(FIFO_WORDCNT) & 0x3F); // 接收完全之后，查fifo有多少字节
            handle_fifo_read(rbuf, &rlen, temp);                       // 读最后的数据
            g_irq_data_in = 0;
            break;
        }
        osal_udelay(DELAY_10_MS);
    }
#endif

#ifdef INTERRUPT
    while (1) {
        handle_interrupts(&irq, &ret, &g_irq_data_wl, &g_irq_data_in);

        if (g_irq_data_in && g_irq_data_wl) {
            g_irq_data_wl = 0;
            handle_fifo_read(rbuf, &rlen, 24); // 渐满之后读取24字节
        }
        if (irq & MAIN_IRQ_RX_DONE) {
            temp = (unsigned int)(fm11_read_reg(FIFO_WORDCNT) & 0x3F); // 接收完全之后，查fifo有多少字节
            handle_fifo_read(rbuf, &rlen, temp);                       // 读最后的数据
            g_irq_data_in = 0;
            break;
        }
        osal_udelay(DELAY_1_MS);
    }
#endif
    if (rlen <= CRC_LEN) {
        return 0;
    }
    rlen -= CRC_LEN; // 2字节crc校验
    return rlen;
}

// 处理 CRC 错误
static void handle_crc_error(unsigned char *crc_err)
{
    unsigned char nak_crc_err = 0x05;
    printf("fm11_t4t: crc_err[1]\n");
    fm11_write_fifo(&nak_crc_err, 1);
    fm11_write_reg(RF_TXEN_REG, 0x55);
    *crc_err = 0;
}

// 处理 INS 为 0xA4 的情况
static void handle_ins_a4(unsigned char *current_file)
{
    unsigned char status_ok[3] = {0x02, 0x90, 0x00};
    unsigned char status_word2[3] = {0x02, 0x6A, 0x00};
    const unsigned char ndef_capability_container[2] = {0xE1, 0x03};
    const unsigned char ndef_id[2] = {0xE1, 0x04};

    status_ok[0] = g_fm327_fifo[0];
    status_word2[0] = g_fm327_fifo[0];
    if (g_fm327_fifo[P1] == 0x00) {
        if ((g_fm327_fifo[LC] == sizeof(ndef_capability_container)) &&
            (memcmp(ndef_capability_container, g_fm327_fifo + DATA, g_fm327_fifo[LC]) == 0)) {
            fm11_write_fifo(status_ok, 0x03);
            fm11_write_reg(RF_TXEN_REG, 0x55);
            *current_file = CC_FILE;
            printf("fm11_t4t: current_file=CC_FILE\n");
        } else if ((g_fm327_fifo[LC] == sizeof(ndef_id)) &&
                   (memcmp(ndef_id, g_fm327_fifo + DATA, g_fm327_fifo[LC]) == 0)) {
            fm11_write_fifo(status_ok, 0x03);
            fm11_write_reg(RF_TXEN_REG, 0x55);
            *current_file = NDEF_FILE;
            printf("fm11_t4t: current_file=NDEF_FILE\n");
        } else {
            fm11_write_fifo(status_word2, 0x03);
            fm11_write_reg(RF_TXEN_REG, 0x55);
            *current_file = NONE;
            printf("fm11_t4t: current_file=NONE\n");
        }
    } else if (g_fm327_fifo[P1] == 0x04) {
        unsigned char ret = fm11_write_fifo(status_ok, 0x03);
        if (ret != 0) {
            printf("fm11_write_reg failed\r\n");
        }
        ret = fm11_write_reg(RF_TXEN_REG, 0x55);
        if (ret != 0) {
            printf("fm11_write_reg failed\r\n");
        }
    } else {
        fm11_write_fifo(status_ok, 0x03);
        fm11_write_reg(RF_TXEN_REG, 0x55);
    }
}

// 处理 INS 为 0xB0 的情况
static void handle_ins_b0(unsigned char current_file)
{
    unsigned char status_ok[3] = {0x02, 0x90, 0x00};
    unsigned char status_word[3] = {0x02, 0x6A, 0x82};
    unsigned char xbuf[256] = {0};
    unsigned char xlen = 0;

    status_ok[0] = g_fm327_fifo[0];
    status_word[0] = g_fm327_fifo[0];
    if (current_file == CC_FILE) {
        printf("fm11_t4t: CC_FILE: fm11_write_fifo...\n");
        fm11_write_fifo(status_ok, 1);
        fm11_write_fifo(g_capability_container + (g_fm327_fifo[P1] << 0x8) + g_fm327_fifo[P2], g_fm327_fifo[LC]);
        fm11_write_fifo(&status_ok[1], 0x02);
        fm11_write_reg(RF_TXEN_REG, 0x55);
    } else if (current_file == NDEF_FILE) {
        memset(&xbuf[0], 0, sizeof(xbuf));
        memcpy(&xbuf[0], &status_ok[0], 1);
        memcpy(&xbuf[1], &g_ndef_file[0] + (g_fm327_fifo[P1] << 0x08) + g_fm327_fifo[P2], g_fm327_fifo[LC]);
        memcpy(&xbuf[0] + g_fm327_fifo[LC] + 1, status_ok + 1, 0x02);
        xlen = g_fm327_fifo[LC] + 0x03;

        printf("fm11_t4t: NDEF_FILE: fm11_data_send: xlen[%d]\n", xlen);
        if (xlen == 0x05) {
            printf("          xbuf[0x%02X] [0x%02X][0x%02X] [0x%02X][0x%02X]\n", xbuf[OFFSET_0], xbuf[OFFSET_1],
                   xbuf[OFFSET_2], xbuf[OFFSET_3], xbuf[OFFSET_3 + 1]);
        } else {
            printf("          xbuf[0x%02X] [0x%02X][0x%02X][0x%02X][0x%02X]...[0x%02X][0x%02X][0x%02X]\n",
                   xbuf[OFFSET_0], xbuf[OFFSET_1], xbuf[OFFSET_2], xbuf[OFFSET_3], xbuf[OFFSET_3 + 1],
                   xbuf[xlen - 0x03], xbuf[xlen - 0x02], xbuf[xlen - 1]);
        }
        fm11_data_send(xlen, xbuf);
    } else {
        fm11_write_fifo(status_word, 0x03);
        fm11_write_reg(RF_TXEN_REG, 0x55);
    }
}

// 处理 INS 为 0xD6 的情况
static void handle_ins_d6(void)
{
    unsigned char status_ok[3] = {0x02, 0x90, 0x00};
    unsigned char i = 0;

    status_ok[0] = g_fm327_fifo[0];
    printf("fm11_t4t: UPDATE_BINARY: rfLen[%d]\n", g_rf_len);
    for (i = 0; i < g_rf_len; i++) {
        printf("0x%02x ", g_fm327_fifo[i]);
    }
    printf("\r\n");
    memcpy(g_ndef_file + (g_fm327_fifo[P1] << 0x08) + g_fm327_fifo[P2], g_fm327_fifo + DATA, g_fm327_fifo[LC]);
    fm11_write_fifo(status_ok, 0x03);
    fm11_write_reg(RF_TXEN_REG, 0x55);
}

/* 写fifo 和 写寄存器 */
void fm11_t4t(void)
{
    unsigned char crc_err = 0;

    if (crc_err) {
        handle_crc_error(&crc_err);
    } else {
        if (g_fm327_fifo[INS] == 0xA4) {
            handle_ins_a4(&g_current_file);
        } else if (g_fm327_fifo[INS] == 0xB0) {
            handle_ins_b0(g_current_file);
        } else if (g_fm327_fifo[INS] == 0xD6) { // UPDATE_BINARY
            handle_ins_d6();
        } else {
            fm11_data_send(g_rf_len, g_fm327_fifo);
        }
    }
}

/* app nfc demo */
void nfcread(void)
{
#ifdef CHECK
    while (1) {
        g_rf_len = fm11_data_recv(g_fm327_fifo); // 读取rf数据(一帧)
        if (g_rf_len > 0) {
            printf("--------------------------------------------------------------\n");
            printf("nfcread : fm327_fifo: [0x%02X]/CLA[0x%02X]/INS[0x%02X]/P1[0x%02X]/P2[0x%02X]/LC[0x%02X]...\n",
                   g_fm327_fifo[0], g_fm327_fifo[CLA], g_fm327_fifo[INS], g_fm327_fifo[P1], g_fm327_fifo[P2],
                   g_fm327_fifo[LC]);
            fm11_t4t();
            g_irq_data_in = 0;
        }
        usleep(DELAY_1_MS);
    }
#endif

#ifdef INTERRUPT
    while (1) {
        if (g_flag_first_frame) {
            rfLen = fm11_data_recv(fm327_fifo); // 读取rf数据(一帧)
            if (rfLen > 0) {
                fm11_t4t();
            }
            g_irq_data_in = 0;
        }
        usleep(1);
    }
#endif
}
