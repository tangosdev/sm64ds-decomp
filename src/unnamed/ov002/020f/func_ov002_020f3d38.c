#include "types.h"
void func_ov002_020f3d38(char *c, int i)
{
    int off = i * 0x4c;
    if (*(u16*)(c + 0x30 + off) != 0) {
        *(u16*)(c + 0x30 + off) = *(u16*)(c + 0x30 + off) - 1;
        return;
    }
    *(s32*)(c + off) = -0x10000;
    *(s32*)(c + off + 4) = -0x10000;
    *(s32*)(c + off + 8) = 0x2000;
    *(u16*)(c + off + 0x2e) = 0;
    *(u16*)(c + 0x30 + off) = 0;
    *(u8*)(c + 0x47 + off) = *(u8*)(c + 0x47 + off) + 1;
}
