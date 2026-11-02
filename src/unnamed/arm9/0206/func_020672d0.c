#include "types.h"
#include "private/wm_views.h"
extern void CpuCopy8(const void *src, void *dst, unsigned int size);

/* On a download-play child, rebuild the parent's WMBssDesc from the copy
 * the boot loader left at 0x027ffc42. The copy is valid when the halfword at
 * 0x027ffc40 is 2, a multiboot child: pret's pokediamond MB_mb.h has this
 * MBParam layout. The name lead is the SDK's MB_ReadMultiBootParentBssDesc,
 * but no pret repository carries its body, so the address name stays. The
 * first 0x3c bytes come over as-is; the game info is then filled in from
 * the arguments, with the parent's ggid and tgid taken from the first six
 * bytes of its SSID. Field names follow the SDK's WMBssDesc/WMGameInfo;
 * this SDK version writes ggid as two halfwords, so the shared view splits it. */

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
