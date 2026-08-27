#include "types.h"
struct S20
{
    s32 a;      /* +0x00 */
    s32 b;      /* +0x04 */
    u8 pad8[4]; /* +0x08 */
    s16 c;      /* +0x0c */
    u8 padE[2]; /* +0x0e */
    u8 d;       /* +0x10 */
    u8 e;       /* +0x11 */
};

struct S24
{
    u8 pad0[0x14]; /* +0x00 */
    u8 a;          /* +0x14 */
    u8 b;          /* +0x15 */
    u8 pad16[2];
};

struct Ctx
{
    u8 pad0000[0x4660];
    struct S20 arr16[16];    /* 0x4660 */
    struct S24 arr2[2];      /* 0x47a0 */
    u8 pad47d0[0x10];        /* 0x47d0 */
    u8 f47e0;                /* 0x47e0 */
    u8 pad47e1[2];
    u8 f47e3;                /* 0x47e3 */
    s32 f47e4;               /* 0x47e4 */
    s32 f47e8;               /* 0x47e8 */
    s32 f47ec;               /* 0x47ec */
    u8 pad47f0[4];
    u8 f47f4;                /* 0x47f4 */
    u8 f47f5;                /* 0x47f5 */
    u8 pad47f6[2];
    s32 arrA[120];           /* 0x47f8 */
    s32 arrB[120];           /* 0x49d8 */
    s32 arrC[120];           /* 0x4bb8 */
    s32 arrD[120];           /* 0x4d98 */
    u8 pad4f78[4];
    s16 arrJ[120];           /* 0x4f7c */
    s16 arrI[120];           /* 0x506c */
    u8 pad515c[8];
    s16 h5164;               /* 0x5164 */
    s16 h5166;               /* 0x5166 */
    s16 h5168;               /* 0x5168 */
    s16 h516a;               /* 0x516a */
    u8 pad516c[0xc];
    u8 grid[13][9];          /* 0x5178 */
    u8 pad51ed[0x10];
    u8 arrE[120];            /* 0x51fd */
    u8 pad5275[120];
    u8 arrF[120];            /* 0x52ed */
    u8 arrG[120];            /* 0x5365 */
    u8 arrH[120];            /* 0x53dd */
    u8 b5455;                /* 0x5455 */
    u8 b5456;                /* 0x5456 */
    u8 pad5457[1];
    u8 b5458;                /* 0x5458 */
    u8 b5459;                /* 0x5459 */
};

void func_ov006_020f2ec0(struct Ctx *c)
{
    s32 i;
    s32 j;
    s32 k;
    s32 m;
    s32 n;

    for (i = 0; i < 120; i++)
    {
        c->arrA[i] = 0;
        c->arrB[i] = 0;
        c->arrC[i] = 0;
        c->arrD[i] = 0;
        c->arrE[i] = 0;
        c->arrF[i] = 0;
        c->arrG[i] = 0;
        c->arrI[i] = 0;
        c->arrJ[i] = 0;
        c->arrH[i] = 0;
    }

    for (j = 0; j < 16; j++)
    {
        c->arr16[j].a = 0;
        c->arr16[j].b = 0;
        c->arr16[j].c = 0;
        c->arr16[j].d = 0;
        c->arr16[j].e = 0;
    }

    for (k = 0; k < 2; k++)
    {
        c->arr2[k].a = 0;
        c->arr2[k].b = 0;
    }

    c->f47e0 = 0;
    c->f47e3 = 0;
    c->f47f4 = 0;
    c->f47e4 = 0;
    c->f47e8 = 0;
    c->f47f5 = 0;
    c->f47ec = 0;

    for (m = 0; m < 13; m++)
    {
        for (n = 0; n < 9; n++)
        {
            c->grid[m][n] = 0;
        }
    }

    c->h5164 = 0;
    c->h5166 = 0x80;
    c->h5168 = 0x60;
    c->h516a = 0;

    c->b5456 = 0;
    c->b5458 = 1;
    c->b5459 = 0;
    c->b5455 = 0;
}
