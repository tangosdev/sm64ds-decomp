#include "types.h"
extern s16 data_02082214[];

void func_ov006_020fe90c(char* base, int i)
{
    int n = i * 0x38;
    s16 t;
    u16 v;
    *(int*)(base + 0x4ef8 + n) = *(int*)(base + 0x4ef8 + n) + *(int*)(base + 0x4efc + n);
    *(int*)(base + 0x4efc + n) = *(int*)(base + 0x4efc + n) + 0x10;
    *(int*)(base + 0x4ed8 + n) = *(int*)(base + 0x4ed8 + n)
        + (int)(((s64)data_02082214[((*(u16*)(base + 0x4f08 + n) >> 4) << 1) + 1] * 0x800 + 0x800) >> 0xc);
    t = data_02082214[(*(u16*)(base + 0x4f08 + n) >> 4) << 1];
    *(int*)(base + 0x4edc + n) = *(int*)(base + 0x4edc + n)
        + (int)(((s64)t * *(int*)(base + 0x4ef8 + n) + 0x800) >> 0xc);
    v = *(u16*)(base + 0x4f0a + n);
    if (v != 0) {
        *(s16*)(base + 0x4f0a + n) = v - 1;
        if (*(s16*)(base + 0x4f0a + n) < 0)
            *(s16*)(base + 0x4f0a + n) = 0;
    } else {
        *(unsigned char*)(base + 0x4f0d + n) = 4;
    }
}
