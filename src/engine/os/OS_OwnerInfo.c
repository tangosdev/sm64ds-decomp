/* The owner's firmware settings (the Nitro SDK's os_ownerInfo.c), arm9
 * .text 0x02059eb0..0x02059f48.
 *
 * Both functions read the copy of the firmware's user settings that the
 * boot code leaves in main memory: the MAC address at 0x027ffcf4 and the
 * NVRAM user-info block at 0x027ffc80.
 *
 * pret's pokediamond OS_ownerInfo.c defines OS_GetMacAddress,
 * OS_GetOwnerInfo and OS_GetOwnerRtcOffset in this order. mwccarm emits the
 * bodies in reverse, so OS_GetOwnerInfo is lowest and OS_GetMacAddress
 * highest. OS_GetOwnerRtcOffset is not in this ROM: nothing calls it, so
 * the linker dropped it.
 */
#include "private/nvram_config.h"

extern void MI_CpuCopy8(const void *src, void *dst, unsigned int size);
extern void MIi_CpuCopy16(const void *src, void *dst, int size);

void OS_GetMacAddress(unsigned char *macAddress) {
    MI_CpuCopy8((void *)0x027ffcf4, macAddress, 6);
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
    NVRAMConfigData *src = (NVRAMConfigData *)0x27ffc80;
    OSOwnerInfo *info = (OSOwnerInfo *)dst;

    info->language = src->language;
    info->favouriteColour = src->owner.favouriteColour;
    info->birthdayMonth = src->owner.birthdayMonth;
    info->birthdayDay = src->owner.birthdayDay;
    info->nickNameLength = src->owner.nickNameLength;
    info->commentLength = src->owner.commentLength;
    MIi_CpuCopy16(src->owner.nickName, info->nickName, sizeof(info->nickName));
    MIi_CpuCopy16(src->owner.comment, info->comment, sizeof(info->comment));
}
