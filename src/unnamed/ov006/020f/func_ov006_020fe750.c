#include "types.h"

extern s16 _ZN4cstd5atan2E5Fix12IiES1_(int y, int x);
extern void func_ov006_020fbbe8(char* c);
extern void func_ov006_020fbd38(void* arg0);

void func_ov006_020fe750(char* c, int idx)
{
    int n = idx * 0x38;
    int old;
    int e, b;

    *(int*)(c + 0x4ed8 + n) = *(int*)(c + 0x4ed8 + n) + *(int*)(c + 0x4ee0 + n);
    *(int*)(c + 0x4edc + n) = *(int*)(c + 0x4edc + n) + *(int*)(c + 0x4ee4 + n);
    *(int*)(c + 0x4efc + n) = *(int*)(c + 0x4efc + n) + 0x10;
    old = *(int*)(c + 0x4ee4 + n);
    *(int*)(c + 0x4ee4 + n) = old + *(int*)(c + 0x4efc + n);
    if (*(int*)(c + 0x4ee4 + n) >= 0x8000)
        *(int*)(c + 0x4ee4 + n) = 0x8000;

    *(s16*)(c + 0x4f08 + n) = _ZN4cstd5atan2E5Fix12IiES1_(
        *(int*)(c + 0x4ee4 + n) >> 12,
        *(int*)(c + 0x4ee0 + n) >> 12);

    if (old < 0) {
        if (*(int*)(c + 0x4ee4 + n) >= 0)
            *(s16*)(c + 0x4f0a + n) = 0x40;
    }

    if (*(u16*)(c + 0x4f0a + n) != 0) {
        *(volatile u16*)(c + 0x4f0a + n) = *(volatile u16*)(c + 0x4f0a + n) - 1;
        if (*(s16*)(c + 0x4f0a + n) <= 0)
            *(u8*)(c + 0x4f0d + n) = 4;
    }

    b = *(int*)(c + 0x4ed8 + n) >> 12;
    e = *(int*)(c + 0x4edc + n) >> 12;
    if (e <= -0x120)
        *(u8*)(c + 0x4f0d + n) = 4;
    if (e >= 0xd0)
        *(u8*)(c + 0x4f0d + n) = 4;
    if (b >= 0x140 || b <= -0x40)
        *(u8*)(c + 0x4f0d + n) = 4;

    func_ov006_020fbbe8(c);
    if (*(u8*)(c + 0x5c2f) != 0)
        func_ov006_020fbd38(c);
}
