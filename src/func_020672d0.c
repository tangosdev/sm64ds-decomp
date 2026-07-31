#include "types.h"
extern void CpuCopy8(const void *src, void *dst, unsigned int size);
void func_020672d0(char *thiz, int a1, int a2, int a3, int a4)
{
    char *base = (char *)0x27ffc40;
    const void *src;
    int flags;
    if (*(u16 *)base == 2)
        src = base + 2;
    else
        src = 0;
    CpuCopy8(src, thiz, 0x3c);
    *(u16 *)(thiz + 0x3c) = 0x10;
    *(u16 *)(thiz + 0x44) = *(u16 *)(thiz + 0xc);
    *(u16 *)(thiz + 0x46) = *(u16 *)(thiz + 0xe);
    *(u16 *)(thiz + 0x48) = *(u16 *)(thiz + 0x10);
    *(u8 *)(thiz + 0x4a) = 0;
    flags = (a3 ? 4 : 0) | (a4 ? 8 : 0);
    *(u16 *)(thiz + 0x4c) = (u16)a1;
    *(u16 *)(thiz + 0x4e) = (u16)a2;
    flags |= 1;
    *(u8 *)(thiz + 0x4b) = (u8)flags;
}
