#ifndef NVRAM_CONFIG_H
#define NVRAM_CONFIG_H

/* The firmware's user settings, which the boot code copies to 0x027ffc80:
 * pret's pokediamond userInfo.h NVRAMConfigData, cut down to what this ROM
 * reads. Moved here from OS_OwnerInfo.c so that TP_GetUserInfo reads the
 * same type. Plain C types: both readers are C files. */

typedef struct NVRAMConfigOwnerInfo {
    unsigned char favouriteColour : 4; /* 0x00 */
    unsigned char rsv : 4;
    unsigned char birthdayMonth; /* 0x01 */
    unsigned char birthdayDay;   /* 0x02 */
    unsigned char pad;
    unsigned short nickName[10];  /* 0x04 */
    unsigned char nickNameLength; /* 0x18 */
    unsigned char nickNameRsv;
    unsigned short comment[26];  /* 0x1a */
    unsigned char commentLength; /* 0x4e */
    unsigned char commentRsv;
} NVRAMConfigOwnerInfo; /* 0x50 */

/* The touch-screen calibration: two raw sample points and the screen dots
 * they were taken at. */
typedef struct NVRAMConfigTpCalibData {
    unsigned short raw_x1; /* 0x00 */
    unsigned short raw_y1; /* 0x02 */
    unsigned char dx1;     /* 0x04 */
    unsigned char dy1;     /* 0x05 */
    unsigned short raw_x2; /* 0x06 */
    unsigned short raw_y2; /* 0x08 */
    unsigned char dx2;     /* 0x0a */
    unsigned char dy2;     /* 0x0b */
} NVRAMConfigTpCalibData;  /* 0x0c */

typedef struct NVRAMConfigData {
    unsigned char version; /* 0x00 */
    unsigned char pad;
    NVRAMConfigOwnerInfo owner;  /* 0x02 */
    unsigned char alarm[6];      /* 0x52, NVRAMConfigAlarm */
    NVRAMConfigTpCalibData tp;   /* 0x58 */
    unsigned short language : 3; /* 0x64, the first field of NVRAMConfigOption */
} NVRAMConfigData;

#endif
