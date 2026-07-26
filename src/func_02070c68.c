typedef unsigned char u8;

extern int func_01ff9d40(int, int, int, int);
extern double func_01ff8e10(int, int, int, int);
extern long long func_020707f4(double, int *);
extern long long func_020708b4(long long, int);
extern int func_02071708(int);
extern long long func_01ff859c(long long);
extern void func_02071510(void *, int, int);
extern void func_02070ec0(void *, int);
extern void func_02071364(void *, void *, void *);

void func_02070c68(char *out, double x)
{
    int e;
    double x1;
    double x2;
    double m;
    unsigned char decimal[0x26];
    unsigned char big[0x26];
    u8 neg;
    int lx;
    int hx;
    int cls;
    int cls2;

    neg = ((*(volatile int *)((char *)&x + 4)) & 0x80000000) != 0;
    lx = ((int *)&x)[0];
    hx = ((int *)&x)[1];

    if (func_01ff9d40(0, 0, lx, hx) != 0) {
        *(u8 *)(out + 0) = neg;
        *(short *)(out + 2) = 0;
        *(u8 *)(out + 4) = 1;
        *(u8 *)(out + 5) = 0;
        return;
    }

    ((int *)&x1)[0] = lx;
    ((int *)&x1)[1] = hx;

    if ((hx & 0x7ff00000) == 0)
        goto lowexp1;
    if ((hx & 0x7ff00000) != 0x7ff00000)
        goto four1;
    if ((hx & 0xfffff) != 0)
        goto nan1;
    if (lx == 0)
        goto inf1;
nan1:
    cls = 1;
    goto have_cls;
inf1:
    cls = 2;
    goto have_cls;
lowexp1:
    if ((hx & 0xfffff) != 0)
        goto sub1;
    if (lx == 0)
        goto zero1;
sub1:
    cls = 5;
    goto have_cls;
zero1:
    cls = 3;
    goto have_cls;
four1:
    cls = 4;
have_cls:
    if (cls <= 2) {
        int l2;
        int h2;

        *(u8 *)(out + 0) = neg;
        *(short *)(out + 2) = 0;
        *(u8 *)(out + 4) = 1;

        x2 = x;
        l2 = ((int *)&x2)[0];
        h2 = ((int *)&x2)[1];

        if ((h2 & 0x7ff00000) == 0)
            goto lowexp2;
        if ((h2 & 0x7ff00000) != 0x7ff00000)
            goto four2;
        if ((h2 & 0xfffff) != 0)
            goto nan2;
        if (l2 == 0)
            goto inf2;
    nan2:
        cls2 = 1;
        goto have_cls2;
    inf2:
        cls2 = 2;
        goto have_cls2;
    lowexp2:
        if ((h2 & 0xfffff) != 0)
            goto sub2;
        if (l2 == 0)
            goto zero2;
    sub2:
        cls2 = 5;
        goto have_cls2;
    zero2:
        cls2 = 3;
        goto have_cls2;
    four2:
        cls2 = 4;
    have_cls2:
        *(u8 *)(out + 5) = (cls2 == 1) ? 0x4e : 0x49;
    } else {
        long long mm;
        long long uu;
        int *mp;
        int nbits;
        int k;

        if (neg)
            x = func_01ff8e10(0, 0, lx, hx);

        mm = func_020707f4(x, &e);
        mp = (int *)&m;
        *(long long *)&m = mm;

        if (mp[0] != 0)
            nbits = func_02071708(mp[0]);
        else
            nbits = func_02071708(mp[1] | 0x100000) + 0x20;

        k = 0x35 - nbits;
        func_02070ec0(big, e - k);

        uu = func_01ff859c(func_020708b4(mm, k));
        func_02071510(decimal, (int)uu, (int)(uu >> 32));

        func_02071364(out, decimal, big);
        *(u8 *)(out + 0) = neg;
    }
}
