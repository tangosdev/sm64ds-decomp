#ifndef NITRO_SPI_USERINFO_H
#define NITRO_SPI_USERINFO_H

/* The firmware's user settings, which the boot code copies to
 * HW_NVRAM_USER_INFO (OS_GetSystemWork()->nvramUserInfo), under NitroSDK's
 * names (nitro/spi/common/userInfo_ts_200.h; pret/pokediamond userInfo.h
 * has the same layout). Read by OS_GetOwnerInfo, OS_GetMacAddress and
 * TP_GetUserInfo. */

#include "types.h"

#define NVRAM_CONFIG_NICKNAME_LENGTH 10
#define NVRAM_CONFIG_COMMENT_LENGTH  26

typedef struct NVRAMConfigDate {
    u8 month; /* 0x00 */
    u8 day;   /* 0x01 */
} NVRAMConfigDate;

typedef struct NVRAMConfigNickname {
    u16 str[NVRAM_CONFIG_NICKNAME_LENGTH]; /* 0x00 */
    u8 length;                             /* 0x14 */
    u8 rsv;
} NVRAMConfigNickname;

typedef struct NVRAMConfigComment {
    u16 str[NVRAM_CONFIG_COMMENT_LENGTH]; /* 0x00 */
    u8 length;                            /* 0x34 */
    u8 rsv;
} NVRAMConfigComment;

typedef struct NVRAMConfigOwnerInfo {
    u8 favoriteColor : 4; /* 0x00 */
    u8 rsv : 4;
    NVRAMConfigDate birthday; /* 0x01 */
    u8 pad;
    NVRAMConfigNickname nickname; /* 0x04 */
    NVRAMConfigComment comment;   /* 0x1a */
} NVRAMConfigOwnerInfo;           /* 0x50 */

typedef struct NVRAMConfigAlarm {
    u8 hour;   /* 0x00 */
    u8 minute; /* 0x01 */
    u8 second; /* 0x02 */
    u8 pad;
    u16 enableWeek : 7; /* 0x04 */
    u16 alarmOn : 1;
    u16 rsv : 8;
} NVRAMConfigAlarm; /* 0x06 */

/* The touch-screen calibration: two raw sample points and the screen dots
 * they were taken at. */
typedef struct NVRAMConfigTpCalibData {
    u16 raw_x1;           /* 0x00 */
    u16 raw_y1;           /* 0x02 */
    u8 dx1;               /* 0x04 */
    u8 dy1;               /* 0x05 */
    u16 raw_x2;           /* 0x06 */
    u16 raw_y2;           /* 0x08 */
    u8 dx2;               /* 0x0a */
    u8 dy2;               /* 0x0b */
} NVRAMConfigTpCalibData; /* 0x0c */

typedef struct NVRAMConfigOption {
    u16 language : 3; /* 0x00 */
    u16 agbLcd : 1;
    u16 detectPullOutCardFlag : 1;
    u16 detectPullOutCtrdgFlag : 1;
    u16 autoBootFlag : 1;
    u16 rsv : 4;
    u16 input_favoriteColor : 1;
    u16 input_tp : 1;
    u16 input_language : 1;
    u16 input_rtc : 1;
    u16 input_nickname : 1;
    u8 timezone;       /* 0x02 */
    u8 rtcClockAdjust; /* 0x03 */
    s64 rtcOffset;     /* 0x04 */
} NVRAMConfigOption;   /* 0x0c */

typedef struct NVRAMConfigData {
    u8 version; /* 0x00 */
    u8 pad;
    NVRAMConfigOwnerInfo owner; /* 0x02 */
    NVRAMConfigAlarm alarm;     /* 0x52 */
    NVRAMConfigTpCalibData tp;  /* 0x58 */
    NVRAMConfigOption option;   /* 0x64 */
} NVRAMConfigData;              /* 0x70 */

typedef struct NVRAMConfig {
    NVRAMConfigData ncd; /* 0x00 */
    u16 saveCount;       /* 0x70 */
    u16 crc16;           /* 0x72 */
} NVRAMConfig;           /* 0x74 */

#endif /* NITRO_SPI_USERINFO_H */
