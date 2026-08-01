#include "types.h"
#define READ8(a) ((((int)(a)) & 1) ? (((*(u16 *)((a) - 1)) & 0xff00) >> 8) : ((*(u16 *)(a)) & 0xff))
#define READ8u(a) ((((int)(a)) & ((unsigned int)1)) ? (((*(u16 *)((a) - 1)) & 0xff00) >> 8) : ((*(u16 *)(a)) & 0xff))
#define WRITE8(a, v) \
    if (((int)(a)) & 1) \
        *(u16 *)((a) - 1) = (*(u16 *)((a) - 1) & 0xff) | (((v) & 0xff) << 8); \
    else \
        *(u16 *)(a) = (*(u16 *)(a) & 0xff00) | ((v) & 0xff)

#pragma opt_strength_reduction off
#pragma optimize_for_size on
#pragma opt_common_subs off

extern int func_0206c8b4(u64 d);

inline char *pad(char *a, int b)
{
    return a + b;
}

inline u64 shr_n(u64 x, int n, int *pj)
{
    int lim;
    int k;
    lim = n;
    for (k = 0; k < lim; k++)
        x >>= 1;
    *pj = k;
    return x;
}

u64 func_0206c51c(u64 a, u64 b)
{
    int tmp;
    int expb;
    int i;
    int j;
    u64 manta;
    u64 mantb;
    u64 out;
    u64 t;
    char *q;
    int bb;
    char *p;
    int exp;

    {
        tmp = func_0206c8b4(a);
        manta = 0;
        exp = tmp;
    }
    expb = func_0206c8b4(b);
    mantb = 0;
    q = (char *)(&a);
    q = pad(q, 6);
    manta |= 0x10;
    manta |= READ8(q) & ((i = 5), 0xf);
    do {
        p = pad((char *)(&a), i);
        manta <<= 8;
        manta |= READ8(p);
    } while (--i >= 0);
    q = (char *)(&b);
    q = pad(q, 6);
    mantb |= 0x10;
    mantb |= READ8(q) & ((i = 5), 0xf);
    do {
        p = pad((char *)(&b), i);
        mantb <<= 8;
        mantb |= READ8(p);
    } while (--i >= 0);
    if (exp == 0 && manta == 0)
        return b;
    if (expb == 0 && mantb == 0)
        return a;
    manta |= 0x0010000000000000ULL;
    mantb |= 0x0010000000000000ULL;
    i = 0;
    if (exp >= expb) {
        for (; i < exp - expb; i++)
            mantb >>= 1;
        {
            volatile u64 *pb = &mantb;
            mantb = manta + *pb;
        }
    } else {
        exp = expb - exp;
        for (; i < exp; i++)
            manta >>= 1;
        exp = expb;
        {
            volatile u64 *pb = &mantb;
            mantb = manta + *pb;
        }
    }
    while (((mantb >> 53) & 1) != 0) {
        mantb >>= 1;
        exp++;
    }
    out = 0;
    q = pad((char *)&out, 7);
    t = READ8(q) | (exp >> 4);
    q = pad((char *)&out, 7);
    WRITE8(q, t);
    q = pad((char *)&out, 6);
    bb = READ8(q);
    bb |= (exp & 0xf) << 4;
    q = pad((char *)&out, 6);
    WRITE8(q, bb);
    mantb &= 0x000fffffffffffffULL;
    for (i = 6; i >= 0; i--) {
        t = shr_n(mantb, i << 3, &j);
        q = pad((char *)&out, i);
        bb = READ8u(q) | ((int)t & 0xff);
        WRITE8(q, bb);
    }
    return out;
}
