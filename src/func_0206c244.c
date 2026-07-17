typedef unsigned short u16;
typedef unsigned long long u64;

#define READ8(a) ((((int)(a)) & 1) ? (((*(u16 *)((a) - 1)) & 0xff00) >> 8) : ((*(u16 *)(a)) & 0xff))
#define WRITE8(a, v) \
    if (((int)(a)) & 1) \
        *(u16 *)((a) - 1) = (*(u16 *)((a) - 1) & 0xff) | (((v) & 0xff) << 8); \
    else \
        *(u16 *)(a) = (*(u16 *)(a) & 0xff00) | ((v) & 0xff)

#pragma opt_strength_reduction off
#pragma optimize_for_size on
#pragma opt_common_subs off

extern int func_0206c90c(u64 d);
extern int func_0206c8b4(u64 d);

inline char *pad(char *a, int b)
{
    return a + b;
}

inline u64 shr_n(u64 x, int n, int *pj)
{
    for (*pj = 0; *pj < n; (*pj)++)
        x >>= 1;
    return x;
}

u64 func_0206c244(u64 d)
{
    int sign;
    int exp;
    int i;
    int j;
    int b;
    u64 t;
    u64 mant;
    u64 out;
    char *q;
    char *p;

    sign = func_0206c90c(d);
    exp = func_0206c8b4(d);
    { volatile u64 *pm = &mant; *pm = 0; }
    q = (char *)&d;
    q = pad(q, 6);
    mant |= 0x10;
    mant |= READ8(q) & ((i = 5), 0xf);
    for (; i >= 0; i--) {
        p = pad((char *)&d, i);
        mant <<= 8;
        mant |= READ8(p);
    }
    mant |= 0x0010000000000000ULL;
    t = mant >> 2;
    exp += 3;
    mant += t;
    while (((mant >> 53) & 1) != 0) {
        mant >>= 1;
        exp++;
    }
    out = 0;
    q = pad((char *)&out, 7);
    t = READ8(q) | (sign << 7);
    q = pad((char *)&out, 7);
    WRITE8(q, t);
    q = pad((char *)&out, 7);
    t = READ8(q) | (exp >> 4);
    q = pad((char *)&out, 7);
    WRITE8(q, t);
    q = pad((char *)&out, 6);
    b = READ8(q);
    b |= (exp & 0xf) << 4;
    q = pad((char *)&out, 6);
    WRITE8(q, b);
    mant &= 0x000fffffffffffffULL;

    for (i = 6; i >= 0; i--) {
        t = shr_n(mant, i << 3, &j);
        q = pad((char *)&out, i);
        b = READ8(q) | ((int)t & 0xff);
        WRITE8(q, b);
    }

    return out;
}
