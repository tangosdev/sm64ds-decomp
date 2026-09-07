#include "types.h"
void func_ov063_02118914(char *c)
{
    int zero = 0;
    u8 copied;
    s32 scale;

    *(u8 *)(c + 0x5cc) = 7;
    *(u8 *)(c + 0x5c8) = zero;
    copied = *(u8 *)(c + 0x5c8);
    *(u8 *)(c + 0x5c9) = copied;
    *(s32 *)(c + 0x584) = 0x1000;
    *(u8 *)(c + 0x5ca) = 3;

    scale = *(s32 *)(c + 0x584);
    *(s32 *)(c + 0x80) = scale;
    *(s32 *)(c + 0x84) = scale;
    *(s32 *)(c + 0x88) = scale;
    *(s32 *)(c + 0x188) = *(s32 *)(c + 0x590) * *(s32 *)(c + 0x584);
    *(s32 *)(c + 0x18c) = *(s32 *)(c + 0x594) * *(s32 *)(c + 0x584);
    *(u16 *)(c + 0x5d4) |= 8;
    *(u32 *)(c + 0x19c) |= 1;
    *(u16 *)(c + 0x5d4) |= 0x100;
    *(s32 *)(c + 0x5c) = -*(s32 *)(c + 0x51c);
    *(s16 *)(c + 0x8e) += 0x8000;
    *(s16 *)(c + 0x94) += 0x8000;
    *(u8 *)(c + 0x5ce) = zero;
    *(u32 *)(c + 0xb0) &= ~2;
}
