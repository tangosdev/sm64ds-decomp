/* The owner's firmware settings (the Nitro SDK's os_ownerInfo.c), arm9
 * .text 0x02059eb0..0x02059f48.
 *
 * Both functions read the copy of the firmware's user settings that the
 * boot code leaves in the system work area (OS_GetSystemWork()->
 * nvramUserInfo): the NVRAMConfig, and the MAC address in the next word-
 * aligned slot after it.
 *
 * pret's pokediamond OS_ownerInfo.c defines OS_GetMacAddress,
 * OS_GetOwnerInfo and OS_GetOwnerRtcOffset in this order. mwccarm emits the
 * bodies in reverse, so OS_GetOwnerInfo is lowest and OS_GetMacAddress
 * highest. OS_GetOwnerRtcOffset is not in this ROM: nothing calls it, so
 * the linker dropped it.
 */
#include "types.h"
#include "nitro/os/systemWork.h"
#include "nitro/spi/userInfo.h"

extern void MI_CpuCopy8(const void *src, void *dst, unsigned int size);
extern void MIi_CpuCopy16(const void *src, void *dst, int size);

void OS_GetMacAddress(unsigned char *macAddress) {
    u8 *src;

    src = (u8 *)((u32)(OS_GetSystemWork()->nvramUserInfo) +
                 ((sizeof(NVRAMConfig) + 3) & ~0x00000003));
    MI_CpuCopy8(src, macAddress, 6);
}

/* The older SDK's 0x50-byte OSOwnerInfo: nickName and comment have no
 * terminator slot. */
typedef struct OSOwnerInfo {
    unsigned char language;
    unsigned char favouriteColour;
    unsigned char birthdayMonth;
    unsigned char birthdayDay;
    unsigned short nickName[10];   /* 0x04 */
    unsigned short nickNameLength; /* 0x18 */
    unsigned short comment[26];    /* 0x1a */
    unsigned short commentLength;  /* 0x4e */
} OSOwnerInfo;

/* Copies the owner fields into an OSOwnerInfo. The parameter stays a byte
 * pointer, the spelling two of its three callers declare. */
void OS_GetOwnerInfo(unsigned char *dst) {
    NVRAMConfig *src = (NVRAMConfig *)(OS_GetSystemWork()->nvramUserInfo);
    OSOwnerInfo *info = (OSOwnerInfo *)dst;

    info->language = src->ncd.option.language;
    info->favouriteColour = src->ncd.owner.favoriteColor;
    info->birthdayMonth = src->ncd.owner.birthday.month;
    info->birthdayDay = src->ncd.owner.birthday.day;
    info->nickNameLength = src->ncd.owner.nickname.length;
    info->commentLength = src->ncd.owner.comment.length;
    MIi_CpuCopy16(src->ncd.owner.nickname.str, info->nickName, sizeof(info->nickName));
    MIi_CpuCopy16(src->ncd.owner.comment.str, info->comment, sizeof(info->comment));
}
