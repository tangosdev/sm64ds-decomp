extern void *fn_0238E688(void);
extern unsigned short *fn_0238E57C(unsigned short *result, unsigned short *a2);
extern unsigned short *fn_0238DE4C(void);
extern int fn_0238DEF4(int a1);

int fn_0238E844(int a1) {
    unsigned short *v2;
    unsigned short buf[6];
    int v4;
    int v5;
    unsigned short s0;
    unsigned short s1;
    unsigned short s2;
    unsigned int t;
    int mask;
    int result;
    int tmp;
    unsigned short *q;
    unsigned short *msg;

    tmp = (int)fn_0238E688() + 0x1000;
    v2 = *(unsigned short **)*(int *)(tmp + 0x610);
    if (*v2 == 7 || *v2 == 9) {
        fn_0238E57C(buf, (unsigned short *)(a1 + 16));
        v4 = 0;
        v5 = 0;
        s0 = buf[0];
        s1 = buf[1];
        s2 = buf[2];
        result = 6;
        while (v5 < 15) {
            q = (unsigned short *)(6 * v5 + (int)v2 + 0x100);
            if (s0 == *(q + 18) && s1 == *(q + 19) && s2 == *(q + 20)) {
                t = (v5 + 1) << 16;
                v4 = t >> 16;
                mask = ~((unsigned)1 << v4);
                *(unsigned short *)((char *)v2 + 0x17E) &= mask;
                v2[67] &= mask;
                result = 0;
                *(q + 18) = 0;
                *(q + 19) = 0;
                *(q + 20) = 0;
                break;
            }
            v5++;
        }
        if (v4 != 0) {
            msg = fn_0238DE4C();
            msg[0] = 8;
            msg[1] = 0;
            msg[4] = 9;
            msg[9] = *(unsigned short *)(a1 + 22);
            msg[8] = v4;
            fn_0238E57C(msg + 5, (unsigned short *)(a1 + 16));
            return fn_0238DEF4((int)msg);
        }
        return result;
    } else {
        ((int *)v2)[4] = 0;
        ((int *)v2)[6] = 0;
        ((int *)v2)[5] = 0;
        msg = fn_0238DE4C();
        msg[0] = 12;
        msg[1] = 0;
        msg[4] = 9;
        msg[6] = *(unsigned short *)(a1 + 22);
        msg[5] = v2[194];
        v2[199] = 0;
        v2[200] = 0;
        return fn_0238DEF4((int)msg);
    }
}
