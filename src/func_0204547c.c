#include "types.h"
typedef struct {
    u8 b0; u8 b1; u16 h2;
    u8 b4; u8 b5; u16 h6;
    u8 b8; u8 b9; u16 hA;
    u8 bC; u8 bD; u16 hE;
    u8 b10; u8 b11; u16 h12;
    u8 b14; u8 b15; u16 h16;
    u8 b18; u8 b19; u16 h1A;
    u8 b1C; u8 b1D; u16 h1E;
    u8 b20; u8 b21; u16 h22;
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

extern int func_020457f0(int v, int a2, int b0, int b1, int *p);
extern int func_020456a0(int v, int a2, int b0, int b1, short *p);

void func_0204547c(Ctx *ctx, Src *s, int a2, Dst *d)
{
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
        d->fC  = func_020457f0(v, a2, s->b0, s->b1, ctx->p8 + s->h2);
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
