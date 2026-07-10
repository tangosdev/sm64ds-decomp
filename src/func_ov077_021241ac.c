typedef long long s64;
typedef short s16;
typedef unsigned short u16;
typedef unsigned char u8;
extern s16 data_02082214[];
#define LA(p) ((int)(((s64)(int)(p)) & 0xFFFFFFFFFFFFFFFFLL))

int func_ov077_021241ac(char *o)
{

    char *d0; int *a5c; int *a60; int *a64; int *src;
    char *p400; int three; int k; s16 s; s16 cval; int v7000; int v1e;

    *(int *)LA(o + 0xb0) &= ~0x80000;

    d0 = *(char **)(o + 0xd0);
    a5c = (int *)LA(o + 0x5c);
    *(int *)(o + 0x98) = *(int *)(d0 + 0x98) + 0xa000;

    d0 = *(char **)(o + 0xd0);
    a60 = (int *)LA(o + 0x60);
    a64 = (int *)LA(o + 0x64);
    *(s16 *)(o + 0x94) = *(s16 *)(d0 + 0x8e);

    d0 = *(char **)(o + 0xd0);

    v7000 = 0x7000;
    src = (int *)LA(d0 + 0x5c);
    *(int *)(o + 0x5c) = src[0];
    p400 = o + 0x400;
    v1e = 0x1e;
    *(int *)(o + 0x60) = src[1];
    *(int *)(o + 0x64) = src[2];
    three = 3;


    k = ((int)*(u16 *)(o + 0x94)) >> 4;
    s = data_02082214[k * 2];
    *a5c = *a5c + (int)(((s64)s * 0x50000 + 0x800) >> 12);
    *a60 = *a60 + 0x50000;
    k = ((int)*(u16 *)(o + 0x94)) >> 4;
    cval = data_02082214[k * 2 + 1];
    *a64 = *a64 + (int)(((s64)cval * 0x50000 + 0x800) >> 12);

    *(s16 *)(p400 + 0x1a) = (s16)v7000;
    *(u8 *)(o + 0x41c) = (u8)v1e;
    *(int *)(o + 0xd0) = 0;
    *(int *)(o + 0x3f4) = three;
    return 1;
}
