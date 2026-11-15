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
#ifndef __APP_DEMO_CONFIG_H__
#define __APP_DEMO_CONFIG_H__

#include "i2c.h"

// 如果想使用中断方式，可以将CHECK改为INTERRUPT
#define CHECK

#define CONFIG_NFC_IRQ GPIO_07 // 中断信号输出，低电平有效
#define CONFIG_NFC_CSN GPIO_09 // 接触端电源开关使能信号(低电平有效)
#define CONFIG_I2C_SCL_MASTER_PIN 15
#define CONFIG_I2C_SDA_MASTER_PIN 16
#define CONFIG_I2C_MASTER_PIN_MODE 2
#define CONFIG_I2C_BUS_ID 1
#define HI_I2C_IDX_BAUDRATE 400000 // 400k

#define NFC_I2C_REG_ARRAY_LEN (32)
#define NFC_SEND_BUFF (3)
#define C08I_NFC_DEMO_TASK_STAK_SIZE (1024 * 10)
#define NFC_DISPLAY_TASK_STAK_SIZE (1024)
#define C08I_NFC_DEMO_TASK_PRIORITY (25)
#define NFC_TAG_WECHAT
#define CLEAN_STOP_SIGNAL (unsigned char(0x00))

#define CLA (1)
#define INS (2)
#define P1 (3)
#define P2 (4)
#define LC (5)
#define DATA (6)

#define C081_NFC_ADDR 0x57 // 7 bit slave device address  1010 111 0/1
#define I2C_WR 0x00
#define I2C_RD 0x01
#define C081_NFC_READ_ADDR 0xAF
#define C081NFC_WRITE_ADDR (C081_NFC_ADDR | I2C_WR)
#define C081NFC_READ_ADDR (C081_NFC_ADDR | I2C_RD)
#define FM11_E2_USER_ADDR 0x0010
#define FM11_E2_MANUF_ADDR 0x03FF
#define FM11_E2_BLOCK_SIZE 16

#define FM327_FIFO 0xFFF0
#define FIFO_FLUSH_REG 0xFFF1
#define FIFO_WORDCNT_REG 0xFFF2
#define RF_STATUS_REG 0xFFF3
#define RF_TXEN_REG 0xFFF4
#define RF_BAUD_REG 0xFFF5
#define RF_RATS_REG 0xFFF6
#define MAIN_IRQ_REG 0xFFF7
#define FIFO_IRQ_REG 0xFFF8
#define AUX_IRQ_REG 0xFFF9
#define MAIN_IRQ_MASK_REG 0xFFFA
#define FIFO_IRQ_MASK_REG 0xFFFB
#define AUX_IRQ_MASK_REG 0xFFFC
#define NFC_CFG_REG 0xFFFD
#define VOUT_CFG_REG 0xFFFE
#define EE_WR_CTRL_REG 0xFFFF

#define MAIN_IRQ 0xFFF7
#define FIFO_IRQ 0xFFF8
#define AUX_IRQ 0xFFF9
#define MAIN_IRQ_MASK 0xFFFA
#define FIFO_IRQ_MASK 0xFFFB
#define AUX_IRQ_MASK 0xFFFC
#define FIFO_FLUSH 0xFFF1
#define FIFO_WORDCNT 0xFFF2

#define MAIN_IRQ_RF_PWON 0x80
#define MAIN_IRQ_ACTIVE 0x40
#define MAIN_IRQ_RX_START 0x20
#define MAIN_IRQ_RX_DONE 0x10
#define MAIN_IRQ_TX_DONE 0x08
#define MAIN_IRQ_ARBIT 0x04
#define MAIN_IRQ_FIFO 0x02
#define MAIN_IRQ_AUX 0x01
#define FIFO_IRQ_WL 0x08

#define RESET_SILENCE 0xFFE6
#define FM441_RF_TXCTL_REG 0xFFF4 // RF_TXEN_REG一样
#define FM441_SERIAL_NUMBER_EEADDRESS 0x000
#define FM441_T0_CONTROL_EEADDRESS 0x3B1
#define FM441_TBCONTROL_EEADDRESS 0x3B5
#define FM441_CRC8_EEADDRESS 0X3BB
#define FM441_ATQA_EEADDRESS 0x3BC
#define FM441_SAK_SAK_CONTROL_EEADDRESS 0x3BF
#define FM441_PROTOCOL_CONTROL_EEADDRESS 0x390

typedef enum {
    TAG_VOUTENABLE_OD_WIP = 0x98,  // 10011000  0x98    tag模式 对外供电   中断为低电平  擦写EEPROM输出中断
    TAG_VOUTENABLE_OD_FD = 0x90,   // 10010000  0x90    tag模式 对外供电   中断为低电平  进场或者选中tag输出中断
    TAG_VOUTENABLE_PP_WIP = 0x88,  // 10001000  0x88    tag模式 对外供电   中断为高电平  擦写EEPROM输出中断
    TAG_VOUTENABLE_PP_FD = 0x80,   // 10000000  0x80    tag模式 对外供电   中断为高电平  进场或者选中tag输出中断
    TAG_VOUTDISABLE_OD_WIP = 0x18, // 00011000  0x18    tag模式 不对外供电 中断为低电平  擦写EEPROM输出中断
    TAG_VOUTDISABLE_OD_FD = 0x10,  // 00010000  0x10    tag模式 不对外供电 中断为低电平  进场或者选中tag输出中断
    TAG_VOUTDISABLE_PP_WIP = 0x08, // 00001000  0x08    tag模式 不对外供电 中断为高电平  擦写EEPROM输出中断
    TAG_VOUTDISABLE_PP_FD = 0x00,  // 00000000  0x00    tag模式 不对外供电 中断为高电平  进场或者选中tag输出中断
    AFE_VOUTENSABLE = 0x91,        // 10010001  0x91    通道模式 对外供电
    AFE_VOUTDISABLE = 0x11         // 00010001  0x11    通道模式 不对外供电
} user_cfg0_typedef;

typedef enum {
    TAG_INVENTORYDISABLE = 0x00, // 00000000  0x00  tag模式 不支持防冲突
    TAG_INVENTORYENABLE_AFE_INVENTORYENABLE_14443_4_RFONENSABLE =
        0x80,                                       // 10000000  0x80  tag模式或者通道模式  14443_4  开启进场中断
    AFE_INVENTORYENABLE_14443_3_RFONENABLE = 0x84,  // 10000100  0x84  通道模式  支持防冲突 14443_3  开启进场中断
    AFE_INVENTORYENABLE_14443_3_RFONDISABLE = 0x86, // 10000110  0x86  通道模式  支持防冲突 14443_3  屏蔽进场中断
    AFE_INVENTORYENABLE_14443_4_RFONDISABLE = 0x82  // 10000010  0x82  通道模式  支持防冲突 14443_4  屏蔽进场中断
} user_cfg1_typedef;

typedef enum {
    TAG_FD_ARBITRATENONE = 0xC1,    // 11000001  0xC1    tag模式 输出FD  CT和RF无仲裁
    TAG_FD_ARBITRATECT = 0xE1,      // 11100001  0xE1    tag模式 输出FD  CT优先
    TAG_SD_ARBITRATENONE = 0x81,    // 10000001  0x81    tag模式 输出SD  CT和RF无仲裁
    TAG_SD_ARBITRATECT = 0xA1,      // 10100001  0xA1    tag模式 输出SD  CT优先
    TAG_WIP_ARBITRATENONE = 0x01,   // 00000001  0x01    tag模式 WIP     CT和RF无仲裁
    AFE_TAG_WIP_ARBITRATECT = 0x21, // 00100001  0x21    通道模式或者 tag模式 WIP CT优先
} user_cfg2_typedef;

typedef struct {
    unsigned char user_check;
    user_cfg0_typedef user_cfg0;
    user_cfg1_typedef user_cfg1;
    user_cfg2_typedef user_cfg2;
} protocol_init_typedef;

typedef enum { NONE, CC_FILE, NDEF_FILE } t4t_file;

typedef enum { NFC_RECOVERY = 0, NFC_CLEAN } nfc_clean_stop_signal;

extern unsigned char g_irq_data_in;
extern unsigned char g_irq_rxdone;
extern unsigned char g_irq_txdone;
extern unsigned char g_flag_first_frame;
extern unsigned char g_isr_flag;
extern unsigned char g_menu_mode;
extern unsigned char g_current_mode;
extern unsigned char g_current_type;
extern unsigned char g_menu_select;
extern unsigned char g_nfc_mode;

unsigned int c08i_nfc_i2c_write(unsigned char reg_high_8bit_cmd,
                                unsigned char reg_low_8bit_cmd,
                                unsigned char *data_buff,
                                unsigned char len);
void fm11_t4t(void);
unsigned int fm11_data_recv(unsigned char *rbuf);
unsigned char fm11_write_reg(unsigned short addr, unsigned char data);
unsigned int fm11_read_eep(unsigned char *data_buff, unsigned short read_addr, unsigned short len);
void fm11_write_eep(unsigned short addr, unsigned int len, unsigned char *wbuf);
void see_write_page(unsigned char *buffer, unsigned short write_addr, unsigned char datalen);
void fm11_data_send(unsigned int ilen, unsigned char *ibuf);
unsigned char fm11_write_fifo(unsigned char *pbuf, unsigned char len);
unsigned int write_read(unsigned char reg_high_8bit_cmd,
                        unsigned char reg_low_8bit_cmd,
                        unsigned char *recv_data,
                        unsigned char send_len,
                        unsigned char read_len);
unsigned char fm11_read_reg(unsigned short addr);
unsigned int write_fifo_reg(unsigned char reg_high_8bit_cmd, unsigned char reg_low_8bit_cmd, unsigned char data_buff);
unsigned int write_fifo_data(unsigned char *data_buff, unsigned char len);
void fm11_init(void);
void protocol_config(void);
void nfcread(void);
void nfc_init(void);
#endif
