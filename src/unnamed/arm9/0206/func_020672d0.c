#include "types.h"
extern void CpuCopy8(const void *src, void *dst, unsigned int size);

/* NitroSDK MB_ReadMultiBootParentBssDesc: on a download-play child, rebuild
 * the parent's WMBssDesc from the copy the boot loader left at 0x027ffc42
 * (valid when the halfword at 0x027ffc40 is 2, a multiboot child). The
 * first 0x3c bytes come over as-is; the game info is then filled in from
 * the arguments, with the parent's ggid and tgid taken from the first six
 * bytes of its SSID. Field names follow the SDK's WMBssDesc/WMGameInfo;
 * this SDK version writes ggid as two halfwords, so the view splits it. */
typedef struct WMBssDesc {
    u8 pad_00[0xc];
    u16 ssid[0x10]; /* 0x0c -- u8[32] in the SDK */
    u8 pad_2c[0x10];
    u16 gameInfoLength; /* 0x3c */
    u8 pad_3e[6];
    struct {
        u16 ggid[2];           /* 0x44 */
        u16 tgid;              /* 0x48 */
        u8 userGameInfoLength; /* 0x4a */
        u8 attribute;          /* 0x4b */
        u16 parentMaxSize;     /* 0x4c */
        u16 childMaxSize;      /* 0x4e */
    } gameInfo;
} WMBssDesc;

void func_020672d0(char *thiz, int parentMaxSize, int childMaxSize, int ksFlag, int csFlag) {
    WMBssDesc *desc = (WMBssDesc *)thiz;
    char *base = (char *)0x27ffc40;
    const void *src;
    int flags;
    if (*(u16 *)base == 2)
        src = base + 2;
    else
        src = 0;
    CpuCopy8(src, thiz, 0x3c);
    desc->gameInfoLength = 0x10;
    desc->gameInfo.ggid[0] = desc->ssid[0];
    desc->gameInfo.ggid[1] = desc->ssid[1];
    desc->gameInfo.tgid = desc->ssid[2];
    desc->gameInfo.userGameInfoLength = 0;
    flags = (ksFlag ? 4 : 0) | (csFlag ? 8 : 0); /* WM_ATTR_FLAG_KS, _CS */
    desc->gameInfo.parentMaxSize = (u16)parentMaxSize;
    desc->gameInfo.childMaxSize = (u16)childMaxSize;
    flags |= 1; /* WM_ATTR_FLAG_ENTRY */
    desc->gameInfo.attribute = (u8)flags;
}
