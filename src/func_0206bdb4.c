typedef unsigned short u16;
typedef unsigned long long u64;

#define READ8(a) ((((int)(a)) & 1) ? (((*(u16 *)((a) - 1)) & 0xff00) >> 8) : ((*(u16 *)(a)) & 0xff))
#define READ8u(a) ((((int)(a)) & ((unsigned int)1)) ? (((*(u16 *)((a) - 1)) & 0xff00) >> 8) : ((*(u16 *)(a)) & 0xff))
#define WRITE8(a, v) \
    if (((int)(a)) & 1) \
        *(u16 *)((a) - 1) = (*(u16 *)((a) - 1) & 0xff) | (((v) & 0xff) << 8); \
    else \
        *(u16 *)(a) = (*(u16 *)(a) & 0xff00) | ((v) & 0xff)

#pragma opt_strength_reduction off
#pragma optimize_for_size on

extern int func_0206c90c(u64 d);
extern int func_0206c8b4(u64 d);

inline char *pad(char *a, int b)
{
    return a + b;
}

u64 func_0206bdb4(u64 d, int mode)
{
    int sign;
    int exp;
    u64 m;
    int i;
    char *q;
    char *p;
    u64 acc;
    int zero;
    int one;
    int c35;
    int lim;
    u64 out;
    unsigned exp2;
    int cnt;
    int fpflag;
    u64 mm;
    int k;
    u64 t;
    int n;
    int j;
    u64 s;
    int b;

    sign = func_0206c90c(d);
    exp = func_0206c8b4(d);
    m = 0;
    q = (char *)(&d);
    q = pad(q, 6);
    m |= 0x10;
    m |= READ8(q) & ((i = 5), 0xf);
    for (; i >= 0; i--) {
        p = pad((char *)(&d), i);
        m <<= 8;
        m |= READ8(p);
    }

    one = 1;
    exp2 = (unsigned)(exp - 0x3fd);
    if (exp >= 3)
        exp -= 3;
    else
        exp = 0;
    c35 = 0x35;
    m |= 0x0010000000000000ULL;
    acc = 0;
    zero = (int)acc;
    cnt = zero;
    i = zero;
    if (mode == 0)
        fpflag = one;
    else
        fpflag = zero;

    while (i < (((int)exp2 <= 0x35 && fpflag) ? (int)exp2 : c35)) {
        acc <<= 1;
        n++;
        n--;
        cnt <<= 1;
        t = m;
        for (k = 0, lim = 0x34 - i; k < lim; k++)
            t >>= 1;
        if ((t & 1) != 0)
            cnt += 1;
        if ((unsigned)cnt >= 5u) {
            acc += 1;
            cnt -= 5;
        }
        i++;
    }
    if (cnt != 0)
        acc += 1;

    n = 0x35 - i;
    for (k = 0; k < n; k++)
        acc <<= 1;

    if ((int)exp2 >= 3)
        exp2 -= 3;
    else
        exp2 = 0;

    m -= acc;

    if (mode == 0) {
        n = 0x35 - (int)exp2;
        for (k = 0; k < n; k++)
            m >>= 1;
        for (k = 0; k < n; k++)
            m <<= 1;
    }

    while (((m >> 52) & 1) == 0 && m != 0) {
        exp -= 1;
        m <<= 1;
    }

    mm = m & 0x000fffffffffffffULL;
    if (mm == 0) {
        if (exp < 0x3ff) {
            if (mode == 0) {
                sign = 0;
                exp = 0;
            }
        }
    }

    out = 0;

    q = pad((char *)(&out), 7);
    b = READ8(q) | (sign << 7);
    q = pad((char *)(&out), 7);
    WRITE8(q, b);

    q = pad((char *)(&out), 7);
    b = READ8(q) | (exp >> 4);
    q = pad((char *)(&out), 7);
    WRITE8(q, b);

    q = pad((char *)(&out), 6);
    b = READ8(q) | ((exp & 0xf) << 4);
    q = pad((char *)(&out), 6);
    WRITE8(q, b);

    for (i = 6; i >= 0; i--) {
        m = mm;
        c35 = i << 3;
        for (k = 0; k < c35; k++)
            m >>= 1;
        p = pad((char *)(&out), i);
        b = READ8u(p) | ((int)m & 0xff);
        p = pad((char *)(&out), i);
        WRITE8(p, b);
    }
    return out;
}
