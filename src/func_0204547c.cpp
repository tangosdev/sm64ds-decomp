//cpp
#include "types.h"
#include "cstd.h"

extern "C" {

typedef struct {
    unsigned char b[3];
} B3;

struct B3c {
    unsigned char b[3];
};

typedef struct {
    u8 b0;
    u8 b1;
    u16 h2;
    u8 b4;
    u8 b5;
    u16 h6;
    u8 b8;
    u8 b9;
    u16 hA;
    u8 bC;
    u8 bD;
    u16 hE;
    u8 b10;
    u8 b11;
    u16 h12;
    u8 b14;
    u8 b15;
    u16 h16;
    u8 b18;
    u8 b19;
    u16 h1A;
    u8 b1C;
    u8 b1D;
    u16 h1E;
    u8 b20;
    u8 b21;
    u16 h22;
} Src;

typedef struct {
    int pad0, pad4, pad8;
    int fC, f10, f14;
    u16 f18;
    u16 h1A, h1C, h1E;
    int f20, f24, f28;
} Dst;

typedef struct {
    u8 pad0, pad1;
    u16 h2;
    u32 pad4;
    int *p8;
    short *pC;
    int *p10;
} Ctx;

extern B3 data_02099f80;
extern const struct B3c data_02099f84;

int func_020457f0(int v, int idx, int shift, int flag, int *tbl) {
    B3 buf;
    int rem;
    int hi;
    int lo;
    int frac;
    int aligned;
    int base;
    int t;

    if (flag == 0)
        return tbl[0];

    if (shift == 0)
        return tbl[idx];

    base = ((v - 1) >> shift) << shift;
    t = idx >> shift;

    if (idx >= base)
        return tbl[t + (idx - base)];

    lo = tbl[t];
    hi = tbl[t + 1];
    aligned = t << shift;
    aligned = flag ? aligned : aligned;
    buf = data_02099f80;

    frac = buf.b[shift];
    rem = idx - aligned;
    frac = frac - rem;

    if (rem != 0)
        return (lo * frac + hi * rem) >> shift;

    return lo;
}

unsigned short func_020456a0(int a0, int a1, int shift, int a3, short *tbl) {
    int i, frac;
    short lo, hi, srange, sfrac;
    struct B3c buf;

    if (a3 == 0)
        return ((unsigned short *)tbl)[0];
    if (shift == 0)
        return ((unsigned short *)tbl)[a1];

    i = a1 >> shift;
    {
        int rounded = ((a0 - 1) >> shift) << shift;
        if (a1 >= rounded)
            return ((unsigned short *)tbl)[i + (a1 - rounded)];
    }

    frac = a1 - (i << shift);
    lo = tbl[i];
    hi = tbl[i + 1];

    buf = data_02099f84;

    sfrac = (short)frac;
    srange = (short)(buf.b[shift] - sfrac);

    if (sfrac == 0)
        return (unsigned short)lo;

    if (cstd::abs(lo - hi) >= 0x800) {
        if (lo > hi)
            hi = (short)(hi + 0x1000);
        else
            lo = (short)(lo + 0x1000);
        return (unsigned short)((unsigned short)((hi * sfrac + lo * srange) >> shift) & 0xfff);
    }
    return (unsigned short)((lo * srange + hi * sfrac) >> shift);
}

void func_0204547c(Ctx *ctx, Src *s, int a2, Dst *d) {
    u16 v = ctx->h2;
    u16 *p;

    d->f18 = 0;
    p = (u16 *)((int)d + 0x18);
    *p |= s->h2;
    *p |= s->h6;
    *p |= s->hA;
    *p |= s->b1;
    *p |= s->b5;
    *p |= s->b9;

    if (d->f18 == 0) {
        d->fC = 0x1000;
        d->f10 = 0x1000;
        d->f14 = 0x1000;
    } else {
        d->fC = func_020457f0(v, a2, s->b0, s->b1, ctx->p8 + s->h2);
        d->f10 = func_020457f0(v, a2, s->b4, s->b5, ctx->p8 + s->h6);
        d->f14 = func_020457f0(v, a2, s->b8, s->b9, ctx->p8 + s->hA);
    }

    d->h1A = func_020456a0(v, a2, s->bC, s->bD, ctx->pC + s->hE) << 4;
    d->h1C = func_020456a0(v, a2, s->b10, s->b11, ctx->pC + s->h12) << 4;
    d->h1E = func_020456a0(v, a2, s->b14, s->b15, ctx->pC + s->h16) << 4;

    d->f20 = func_020457f0(v, a2, s->b18, s->b19, ctx->p10 + s->h1A);
    d->f24 = func_020457f0(v, a2, s->b1C, s->b1D, ctx->p10 + s->h1E);
    d->f28 = func_020457f0(v, a2, s->b20, s->b21, ctx->p10 + s->h22);
}
}
