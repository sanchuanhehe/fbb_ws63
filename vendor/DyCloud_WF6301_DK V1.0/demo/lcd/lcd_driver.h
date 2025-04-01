

#ifndef _LCD_DRIVER_H_
#define _LCD_DRIVER_H_

#ifdef __cplusplus
extern "C" {
#endif

#define SCREEN_VERSION      0
#define BEGIN_FLAG          0x12345678
#define END_FLAG            0x87654321
#define _BEGIN_             ((BEGIN_FLAG>>24)&0xff),((BEGIN_FLAG>>16)&0xff),((BEGIN_FLAG>>8)&0xff),(BEGIN_FLAG&0xff)
#define _END_               ((END_FLAG>>24)&0xff),((END_FLAG>>16)&0xff),((END_FLAG>>8)&0xff),(END_FLAG&0xff)

#define REGFLAG_DELAY_FLAG  0xff5aa5ff
#define REGFLAG_DELAY       ((REGFLAG_DELAY_FLAG>>24)&0xff),((REGFLAG_DELAY_FLAG>>16)&0xff),((REGFLAG_DELAY_FLAG>>8)&0xff),(REGFLAG_DELAY_FLAG&0xff)


#ifdef __cplusplus
} /*extern "C"*/
#endif
#endif

