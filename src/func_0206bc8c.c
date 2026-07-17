typedef unsigned short u16;
typedef unsigned long long u64;

#define READ8(a) ((((int)(a)) & 1) ? (((*(u16 *)((a) - 1)) & 0xff00) >> 8) : ((*(u16 *)(a)) & 0xff))

#pragma opt_strength_reduction off
#pragma optimize_for_size on

extern int func_0206c8b4(u64 d);

inline char *pad(char *a, int b)
{
    return a + b;
}

/* goto-pin so if-conversion emits movgt-then-movle (notes §6o) */
inline int imin(int a, int b)
{
    if (a > b)
        goto retb;
    return a;
retb:
    return b;
}

int func_0206bc8c(u64 d, int flag)
{
    int zero;
    unsigned digit;
    int max;
    int j;
    u64 mant;
    int idx;
    int i;
    u64 t;
    int exp;
    char *p;
    int count;
    char *q;

    exp = func_0206c8b4(d);
    mant = 0;
    q = (char *)(&d);
    q = pad(q, 6);
    mant |= 0x10;
    mant |= READ8(q) & ((i = 5), 0xf);
    for (; i >= 0; i--) {
        p = pad((char *)(&d), i);
        mant <<= 8;
        mant |= READ8(p);
    }

    idx = exp - 0x3fe;
    max = 0x35;
    digit = 0;
    zero = (int)digit;
    i = (int)digit;
    j = (int)digit;
    for (; i < imin(idx, max); i++) {
        digit <<= 1;
        t = mant;
        count = 0x34 - i;
        for (j = 0; j < count; j++)
            t >>= 1;
        if ((t & 1) != 0)
            digit++;
        if (digit >= 10u)
            digit -= 10u;
    }
    return digit;
}
