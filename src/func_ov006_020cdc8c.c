#pragma opt_propagation off
typedef int s32;
typedef unsigned short u16;

extern int _ZN4cstd4fdivEii(int a, int b);

void func_ov006_020cdc8c(char *self)
{
    int d;
    int v;
    int base = 0x400;
    u16 *pa = (u16 *)(((long long)(int)(self + 0x9a)) & 0xFFFFFFFFFFFFFFFFLL);

    *pa = *pa + 0x40;

    d = *(s32 *)(self + 0xc) - 0x74000;
    if (d < 0)
        d = -d;
    v = (_ZN4cstd4fdivEii(d, 0x4c000) >> 3) + base;

    if (*(u16 *)(self + 0x9a) & 0x4000)
        *(s32 *)(self + 0x2c) = v;
    else
        *(s32 *)(self + 0x2c) = -v;
}
