#include "types.h"
extern u32 data_020a1fc0[];
extern void func_02041930(void);
extern void func_020650d8(int a, int b, int c);
extern void func_02065050(int a, int b);

void func_020417d0(char *p)
{
    char *g = (char *)data_020a1fc0;
    char *base = g + 0x440;

    if (*(u32 *)(g + 0xc) == 0) return;

    switch (*(u16 *)(p + 4)) {
    case 7:
        if (*(u32 *)g == 0) return;
        if (*(u32 *)(g + 8) != 0) return;
        func_02041930();
        return;
    case 9:
        if (*(u32 *)g == 0) return;
        *(u32 *)(((s32)base + 0x2718) & 0xFFFFFFFFFFFFFFFFull) &= ~(1 << *(u16 *)(p + 0x12));
        return;
    case 0x15: {
        s32 r0 = *(s32 *)(p + 0xc);
        u32 arg2 = *(u16 *)(p + 0x10);
        if (*(u32 *)g != 0)
        {
            u32 bit = *(u16 *)(p + 0x12);
            *(u32 *)(((s32)base + 0x2718) & 0xFFFFFFFFFFFFFFFFull) |= (1 << bit);
            func_020650d8(r0, arg2, bit);
        }
        else
        {
            func_02065050(r0, arg2);
        }
        return;
    }
    }
}
