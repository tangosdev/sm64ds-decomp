extern int func_01ff9d40(double, double);
extern double func_01ff8e10(double, double);
extern double func_020707f4(double, int *);
extern double func_020708b4(double, int);
extern int func_02071708(int);
extern unsigned long long func_01ff859c(double);
extern void func_02071510(void *, unsigned long long);
extern void func_02070ec0(void *, int);
extern void func_02071364(void *, void *, void *);

void func_02070c68(char *out, double x)
{
    int e;
    double t1;
    double t2;
    double mm;
    unsigned char dec[0x26];
    unsigned char big[0x26];
    double v = x;
    unsigned char neg = (((int *)&x)[1] & 0x80000000) != 0;
    int cls;
    int cls2;

    if (func_01ff9d40(0.0, v) != 0) {
        out[0] = neg;
        *(short *)(out + 2) = 0;
        out[4] = 1;
        out[5] = 0;
        return;
    }

    t1 = v;

    if ((((int *)&t1)[1] & 0x7ff00000) != 0) {
        if ((((int *)&t1)[1] & 0x7ff00000) == 0x7ff00000) {
            if ((((int *)&t1)[1] & 0xfffff) != 0) goto n1;
            if (((int *)&t1)[0] == 0) goto i1;
        n1: cls = 1; goto d1;
        i1: cls = 2; goto d1;
        }
    } else {
        if ((((int *)&t1)[1] & 0xfffff) != 0) goto s1;
        if (((int *)&t1)[0] == 0) goto z1;
    s1: cls = 5; goto d1;
    z1: cls = 3; goto d1;
    }
    cls = 4;
d1:
    if (cls <= 2) {
        out[0] = neg;
        *(short *)(out + 2) = 0;
        out[4] = 1;

        t2 = x;
        if ((((int *)&t2)[1] & 0x7ff00000) != 0) {
            if ((((int *)&t2)[1] & 0x7ff00000) == 0x7ff00000) {
                if ((((int *)&t2)[1] & 0xfffff) != 0) goto n2;
                if (((int *)&t2)[0] == 0) goto i2;
            n2: cls2 = 1; goto d2;
            i2: cls2 = 2; goto d2;
            }
        } else {
            if ((((int *)&t2)[1] & 0xfffff) != 0) goto s2;
            if (((int *)&t2)[0] == 0) goto z2;
        s2: cls2 = 5; goto d2;
        z2: cls2 = 3; goto d2;
        }
        cls2 = 4;
    d2:
        out[5] = (cls2 == 1) ? 0x4e : 0x49;
    } else {
        double m;
        int nbits, k;
        int *mp;

        if (neg) x = func_01ff8e10(0.0, v);
        m = func_020707f4(x, &e);
        mm = m;
        mp = (int *)&mm;
        if (mp[0] != 0)
            nbits = func_02071708(mp[0]);
        else
            nbits = func_02071708(mp[1] | 0x100000) + 0x20;
        k = 0x35 - nbits;
        func_02070ec0(big, e - k);
        func_02071510(dec, func_01ff859c(func_020708b4(m, k)));
        func_02071364(out, dec, big);
        out[0] = neg;
    }
}
