#include "types.h"
typedef struct SrcA {
    int f0, f4, f8, fc, f10;
    u16 f14, f16;
    int f18, f1c, f20, f24, f28, f2c;
} SrcA;

typedef struct ElemA {
    int f0, f4, f8, fc;
    u16 f10, f12;
    int f14, f18, f1c, f20, f24, f28, f2c;
} ElemA;

typedef struct SrcB {
    int f0, f4;
    s16 f8, fa;
    int fc, f10, f14, f18;
    u16 f1c, f1e, f20, f22;
    int f24, f28, f2c, f30, f34, f38, f3c;
} SrcB;

typedef struct ElemB {
    int f0, f4, f8, fc, f10, f14;
    u16 f18, f1a, f1c, f1e;
    int f20, f24, f28, f2c;
    u16 f30, f32;
} ElemB;

typedef struct T14 {
    int f0, f4, f8, fc, f10;
} T14;

typedef struct T16 {
    int f0, f4, f8, fc;
} T16;

typedef struct Desc {
    int f0;
    u32 f4;
    SrcB *f8;
    int fc, f10, f14;
    T14 *f18;
    int f1c;
    T16 *f20;
    u32 f24;
    SrcA *f28;
    int f2c;
    int f30;
} Desc;

typedef struct Dst {
    Desc *f0;
    ElemA *f4;
    ElemB *f8;
    ElemA *fc;
    int f10;
} Dst;

void Crash(void);

void func_020462d0(Dst *d, Desc *h, ElemA *arr)
{
    u32 i;
    ElemA *p;
    int t14;
    SrcB *s2;
    ElemB *e2;
    u32 o40;
    u32 o34;
    u32 j;

    p = arr;
    d->f0 = h;
    d->f10 = 0;
    d->fc = p;
    p += h->f4 + h->f30;
    d->f4 = p;
    p += h->f24;
    t14 = 0x14;
    for (i = 0; i < h->f24; i++) {
        SrcA *s;
        ElemA *e;
        u32 shift;
        int idx2;
        int c1;
        s = &h->f28[i];
        e = &d->f4[i];
        e->f0 = s->f4;
        e->f4 = s->f8;
        c1 = h->f28[i].f4;
        e->f1c = s->f20;
        if (c1 != -1) {
            u32 v = *(int *)((int)h->f18 + c1 * t14 + 0x10);
            shift = (v >> 0x1a) & 7;
            *(int *)(((int)e + 0x1c)) |= v;
        }
        idx2 = h->f28[i].f8;
        if (idx2 != -1) {
            u32 v2;
            if (h->f20 == 0)
                Crash();
            v2 = h->f20[idx2].fc;
            if (shift == 2)
                e->f20 = v2 >> 3;
            else
                e->f20 = v2 >> 4;
        }
        e->f24 = s->f24;
        e->f28 = s->f28;
        e->f2c = s->f2c;
        e->f8 = s->fc;
        e->fc = s->f10;
        e->f10 = s->f14;
        e->f14 = s->f18;
        e->f18 = s->f1c;
    }
    d->f8 = (ElemB *)p;
    j = 0;
    if (j < h->f4) {
        o40 = 0;
        o34 = 0;
        do {
            s2 = (SrcB *)((int)h->f8 + o40);
            e2 = (ElemB *)((int)d->f8 + o34);
            e2->f18 = 0;
            if (s2->f10 != 0x1000)
                *(u16 *)(((int)e2 + 0x18)) |= 1;
            if (s2->f14 != 0x1000)
                *(u16 *)(((int)e2 + 0x18)) |= 1;
            if (s2->f18 != 0x1000)
                *(u16 *)(((int)e2 + 0x18)) |= 1;
            *(u16 *)(((int)e2 + 0x18)) |= e2[s2->f8].f18;
            e2->fc = s2->f10;
            e2->f10 = s2->f14;
            e2->f14 = s2->f18;
            e2->f1a = s2->f1c << 4;
            e2->f1c = s2->f1e << 4;
            e2->f1e = s2->f20 << 4;
            e2->f20 = s2->f24;
            e2->f24 = s2->f28;
            e2->f28 = s2->f2c;
            e2->f2c = 0;
            e2->f30 = 0;
            o40 += 0x40;
            o34 += 0x34;
            j++;
        } while (j < h->f4);
    }
}
