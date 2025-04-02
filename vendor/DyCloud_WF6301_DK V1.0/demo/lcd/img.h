/*
 * @Description:
 * @Author: Condi
 * @Date: 2025-03-26 13:58:31
 * @FilePath: \src\application\samples\peripheral\lcd\img.h
 * @LastEditTime: 2025-03-26 13:58:41
 * @LastEditors: Condi
 */
#ifndef IMG_H
#define IMG_H

#include "stdio.h"
#include "stdint.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    uint32_t cf : 5;          /*Color format: See `lv_img_color_format_t`*/
    uint32_t always_zero : 3; /*It the upper bits of the first byte. Always zero to look like a
                               non-printable character*/
    uint32_t reserved : 1;    /*Reserved to be used later*/
    uint32_t compress : 1;    /* image compress */

    uint32_t w : 11; /*Width of the image map*/
    uint32_t h : 11; /*Height of the image map*/
} img_header_t;

typedef struct {
    img_header_t header; /**< A header describing the basics of the image*/
    uint32_t data_size;  /**< Size of the image in bytes*/
    const uint8_t *data; /**< Pointer to the data of the image*/
} img_dsc_t;

#ifdef __cplusplus
}
/*extern "C"*/
#endif

#endif
