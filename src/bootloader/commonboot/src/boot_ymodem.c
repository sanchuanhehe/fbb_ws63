/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Description: ymodem
 *
 * Create: 2023-01-09
 */

#include "boot_errcode.h"
#include "boot_def.h"
#include "boot_crc.h"
#include "boot_delay.h"
#include "boot_serial.h"
#include "boot_watchdog.h"
#include "securec.h"
#include "boot_ymodem.h"
#ifndef CONFIG_LOADERBOOT_SUPPORT_DYNAMIC_PACKET_SIZE
#define YMODEM_PACKET_SIZE  1024    /* packet size: 1024 Bytes */
#else
#define YMODEM_PACKET_SIZE  CONFIG_YMODEM_PACKET_BUFFER_SZIE
#endif
#define YMODEM_SOH          0x01    /* start packet, size 128 Bytes */
#define YMODEM_STX          0x02    /* data packet, size 1024 Bytes */
#define YMODEM_EOT          0x04    /* End of transfer */
#define YMODEM_ACK          0x06    /* receive success */
#define YMODEM_STX_A        0x0a
#define YMODEM_STX_B        0x0b
#define YMODEM_STX_C        0x0c
#define YMODEM_CAN          0x18    /* receiver cancel */
#define YMODEM_C            0x43    /* asscii 'C' */
#define YMODEM_MAX_RETRIES  20
#define YMODEM_CAN_MAX      3
#define YMODEM_LEN_SHORT    128
#define YMODEM_LEN_1K       1024
#define YMODEM_LEN_2K       2048
#define YMODEM_LEN_4K       4096
#define YMODEM_LEN_8K       8192
#define YMODEM_DELAY        20      /* 20 us */
#define YMODEM_DELAY_MAX    1000000   /* 50000 * 20us value 1s */

typedef struct {
    uint32_t packet_len;
    uint8_t packet[YMODEM_PACKET_SIZE];
    bool rx_eof;
    bool tx_ack;
    uint8_t blk;
    uint8_t expected_blk;
    uint32_t file_length;
    uint32_t read_length;
} ymodem_context;

static ymodem_context g_ymodem;

static uint32_t ymodem_get_packet(void);
static int32_t ymodem_getc(void);
static uint32_t ymodem_getc_timeout(uint8_t *ch);
static void ymodem_flush(void);
static uint32_t ymodem_parse_length(const uint8_t *buffer, uint32_t *length, uint32_t parse_max);

static uint32_t is_hex(int8_t ch)
{
    if (((ch >= '0') && (ch <= '9')) ||
        ((ch >= 'A') && (ch <= 'F')) ||
        ((ch >= 'a') && (ch <= 'f'))) {
        return ERRCODE_SUCC;
    }

    return ERRCODE_FAIL;
}

static uint32_t convert_hex(int8_t ch)
{
    uint32_t val = 0;

    if ((ch >= '0') && (ch <= '9')) {
        val = (uint32_t)(int32_t)(ch - '0');
    } else if ((ch >= 'a') && (ch <= 'f')) {
        val = (uint32_t)(int32_t)(ch - 'a' + 10); /* 10: add 10 */
    } else if ((ch >= 'A') && (ch <= 'F')) {
        val = (uint32_t)(int32_t)(ch - 'A' + 10); /* 10: add 10 */
    }

    return val;
}

int32_t ymodem_getc(void)
{
    if (uapi_uart_rx_fifo_is_empty(g_hiburn_uart) == false) {
        return serial_getc();
    }
    return -1;
}

uint32_t ymodem_getc_timeout(uint8_t *ch)
{
    return serial_getc_timeout(YMODEM_DELAY_MAX, ch);
}

static uint32_t ymodem_get_head_process(void)
{
    uint32_t ret = ERRCODE_FAIL, hdr_ch_cnt = 0, can_count = 0;
    uint8_t ch = 0xff;     /* set to invalid value 0xff. */
    bool hdr_flag = false;
    while (hdr_flag == false) {
        ret = ymodem_getc_timeout(&ch);
        if (ret != ERRCODE_SUCC) {
            ymodem_flush();
            mdelay(20); /* delay 20 ms. */
            return ERRCODE_BOOT_YMODEM_TIMEOUT;
        }

        hdr_ch_cnt++;
        switch (ch) {
            case YMODEM_SOH:
                hdr_flag = true;
                g_ymodem.packet_len = YMODEM_LEN_SHORT;
                break;
            case YMODEM_STX:
                hdr_flag = true;
                g_ymodem.packet_len = YMODEM_LEN_1K;
                break;
            case YMODEM_STX_A:
                hdr_flag = true;
                g_ymodem.packet_len = YMODEM_LEN_2K;
                break;
            case YMODEM_STX_B:
                hdr_flag = true;
                g_ymodem.packet_len = YMODEM_LEN_4K;
                break;
            case YMODEM_STX_C:
                hdr_flag = true;
                g_ymodem.packet_len = YMODEM_LEN_8K;
                break;

            case YMODEM_CAN:
                if (++can_count == YMODEM_CAN_MAX) {
                    return ERRCODE_BOOT_YMODEM_CANCEL;
                }

                /* wait for more CAN */
                break;

            case YMODEM_EOT:
                if (hdr_ch_cnt == 1) {
                    serial_putc(YMODEM_ACK);
                    return ERRCODE_BOOT_YMODEM_EOT;
                }

                break;
            default:
                break;
        }
    }
    return ERRCODE_SUCC;
}

uint32_t ymodem_get_packet(void)
{
    uint8_t crc_lo = 0xff; /* set to invalid value 0xff. */
    uint8_t crc_hi = 0xff; /* set to invalid value 0xff. */
    uint8_t blk = 0xff;    /* set to invalid value 0xff. */
    uint8_t cblk = 0xff;   /* set to invalid value 0xff. */
    uint32_t ret = ERRCODE_FAIL;

    uint32_t i = 0;

    if (g_ymodem.tx_ack == true) {
        serial_putc(YMODEM_ACK);
        g_ymodem.tx_ack = false;
    }

    ret = ymodem_get_head_process();
    if (ret != ERRCODE_SUCC) {
        return ret;
    }

    /* header found, start read data */
    ret = ymodem_getc_timeout(&blk);
    if (ret == ERRCODE_FAIL) {
        return ERRCODE_BOOT_YMODEM_TIMEOUT;
    }

    ret = ymodem_getc_timeout(&cblk);
    if (ret == ERRCODE_FAIL) {
        return ERRCODE_BOOT_YMODEM_TIMEOUT;
    }
    udelay(3); /* delay 3 us. */
    for (i = 0; i < g_ymodem.packet_len; i++) {
        ret = ymodem_getc_timeout(&g_ymodem.packet[i]);
        if (ret == ERRCODE_FAIL) {
            return ret;
        }
    }
    ret = ymodem_getc_timeout(&crc_hi);
    if (ret == ERRCODE_FAIL) {
        return ERRCODE_BOOT_YMODEM_TIMEOUT;
    }

    ret = ymodem_getc_timeout(&crc_lo);
    if (ret == ERRCODE_FAIL) {
        return ERRCODE_BOOT_YMODEM_TIMEOUT;
    }

    /* verify the packet */
    if ((blk ^ cblk) != 0xFF) {
        return ERRCODE_BOOT_YMODEM_FRAME;
    }

    uint16_t cs = crc16_ccitt(0, g_ymodem.packet, g_ymodem.packet_len);
    if (cs != makeu16(crc_lo, crc_hi)) {
        return ERRCODE_BOOT_YMODEM_CS;
    }

    g_ymodem.blk = (uint8_t)blk;

    return ERRCODE_SUCC;
}

static uint32_t ymodem_get_info(void)
{
    if (g_ymodem.blk == 0) {
        udelay(3); /* delay 3 us. */
        uint8_t *buffer = g_ymodem.packet;
        uint32_t counter_times = YMODEM_LEN_SHORT;
        /* skip file name */
        while (*buffer++ != '\0') {
            if (counter_times-- <= 0) {
                break;
            }
        }
        /* get file length */
        if (ymodem_parse_length((const uint8_t *)buffer, &g_ymodem.file_length, YMODEM_LEN_SHORT) != ERRCODE_SUCC) {
            return ERRCODE_FAIL;
        }
        if (g_ymodem.file_length < LOAD_MIN_SIZE || g_ymodem.file_length > LOAD_MAX_SIZE) {
            return ERRCODE_BOOT_YMODEM_LENTH;
        }
        g_ymodem.tx_ack = true;
    }
    g_ymodem.expected_blk = 1;
    g_ymodem.packet_len = 0;
    return ERRCODE_SUCC;
}

uint32_t ymodem_open(void)
{
    g_ymodem.packet_len = 0;
    g_ymodem.file_length = 0;
    g_ymodem.read_length = 0;
    g_ymodem.tx_ack = false;
    g_ymodem.rx_eof = false;

    serial_putc(YMODEM_C);
    int32_t retries = YMODEM_MAX_RETRIES;
    boot_wdt_kick();
    uint32_t ret = 0;

    while (retries-- > 0) {
        ret = ymodem_get_packet();
        if (ret == ERRCODE_SUCC) {
            return ymodem_get_info();
        }
        if (ret == ERRCODE_BOOT_YMODEM_CANCEL) {
            break;
        }
        if (ret == ERRCODE_BOOT_YMODEM_TIMEOUT) {
            if (retries < 0) {
                return ERRCODE_BOOT_YMODEM_TIMEOUT;
            }
            boot_wdt_kick();
            mdelay(20); /* delay 20 ms */
            serial_putc(YMODEM_C);
        }
    }
    return ERRCODE_FAIL;
}

void ymodem_close(void)
{
    for (;;) {
        if (ymodem_getc() < 0) {
            break;
        }
    }
}

static void get_packet_len(void)
{
    g_ymodem.tx_ack = true;
    g_ymodem.expected_blk = (g_ymodem.expected_blk + 1) & 0xff;
    g_ymodem.read_length += (uint32_t)g_ymodem.packet_len;
    if (g_ymodem.read_length > g_ymodem.file_length) {
        g_ymodem.packet_len -= (uint32_t)(g_ymodem.read_length - g_ymodem.file_length);
    }
    return;
}

static uint32_t ymodem_check_packet(void)
{
    uint32_t ret = ERRCODE_FAIL;
    int32_t retries = YMODEM_MAX_RETRIES;
    while (retries-- > 0) {
        ret = ymodem_get_packet();
        if (ret == ERRCODE_SUCC) {
            if (g_ymodem.blk == g_ymodem.expected_blk) {
                get_packet_len();
                break;
            } else if (g_ymodem.blk == ((g_ymodem.expected_blk - 1) & 0xff)) {
                serial_putc(YMODEM_ACK);
                continue;
            } else {
                ret = ERRCODE_BOOT_YMODEM_SEQ;
            }
        }
        if (ret == ERRCODE_BOOT_YMODEM_CANCEL) {
            break;
        }
        if (ret == ERRCODE_BOOT_YMODEM_EOT) {
            serial_putc(YMODEM_ACK);
            serial_putc(YMODEM_C);
            ret = ymodem_get_packet();
            serial_putc(YMODEM_ACK);

            g_ymodem.rx_eof = true;
            break;
        }
        ymodem_flush();
        serial_putc(YMODEM_C);
        boot_wdt_kick();
    }
    return ret;
}

uint32_t ymodem_read(uint8_t* buf, uint32_t size)
{
    uint8_t* read_buf = buf;
    uint32_t read_size = size;
    uint32_t rx_total = 0;
    /* read 'read_size' bytes into 'buf' */
    while ((g_ymodem.rx_eof == false) && (read_size > 0)) {
        if (g_ymodem.packet_len == 0) {
            if (ymodem_check_packet() != ERRCODE_SUCC) {
                return rx_total;
            }
        }

        if (g_ymodem.rx_eof == false) {
            uint32_t length = min(read_size, g_ymodem.packet_len);

            errcode_t ret = memcpy_s(read_buf, (uint32_t)read_size, g_ymodem.packet, length);
            if (ret != EOK) {
                return ERRCODE_FAIL;
            }
            read_size -= length;
            read_buf += length;
            rx_total += length;
            g_ymodem.packet_len = 0;
        }
    }
    return rx_total;
}

uint32_t ymodem_parse_length(const uint8_t *buffer, uint32_t *length, uint32_t parse_max)
{
    const uint8_t *read_buffer = buffer;
    uint32_t value = 0;
    uint32_t radix = DECIMAL;
    uint32_t count = parse_max;
    if (read_buffer == NULL || length == NULL) {
        return ERRCODE_FAIL;
    }

    /* skip 'space' */
    while (*read_buffer == ' ' && count > 0) {
        read_buffer++;
        count--;
    }
     /* 1: makesure >= 2 */
    if ((read_buffer[0] == '0') && ((read_buffer[1] == 'x') || (read_buffer[1] == 'X')) && count > 1) {
        radix = HEXADECIMAL;
        read_buffer += 2; /* 2: add 2 */
        count -= 2;  /* 2: sub 2 */
    }

    while (*read_buffer != '\0' && count > 0) {
        int8_t ch = (int8_t)*read_buffer++;
        if (is_hex(ch) == ERRCODE_FAIL) {
            return ERRCODE_FAIL;
        }

        uint32_t tmp = convert_hex(ch);
        if (tmp <= radix) {
            value *= radix;
            value += tmp;
        }
    }

    *length = value;

    return ERRCODE_SUCC;
}

uint32_t ymodem_get_length(void)
{
    return g_ymodem.file_length;
}

void ymodem_flush(void)
{
    for (;;) {
        uint8_t ch = 0xff;
        uint32_t ret = ymodem_getc_timeout(&ch);
        if (ret == ERRCODE_FAIL) {
            return;
        }
    }
}