extern int *fn_0238E688(void);
extern int fn_0238E184(int);
extern void fn_02382DC0(void *, void *, unsigned int);
extern int fn_02382AC4(void);
extern void fn_0238219C(void *);
extern int fn_0238D594(int, int);
extern void fn_023822A8(void *, int, int, void *, int);
extern int fn_02382AD8(int);
extern int fn_02393128(int, int);
extern short *fn_0238DE4C(void);
extern int fn_0238DEF4(short *);
extern void fn_0238E57C(void *, void *);
extern int fn_02392F18(int, int, void *, int, void *);
extern int fn_0238DEA8(void);
extern void fn_0238EFCC(void);

void fn_0238F050(unsigned char *a1) {
    int sb;
    unsigned char *s;
    unsigned char *p;
    int r6;
    int r5;
    int r4;
    int *o;
    volatile short *msg;
    int v;
    int b1, b2;
    int c6;
    unsigned short f;

    o = fn_0238E688();
    s = **(unsigned char ***)((char *)o + 0x1610);

    v = fn_0238E184(a1[0x1F]);
    if (*(unsigned short *)(s + 0xBA) > (unsigned int)v)
        *(unsigned short *)(s + 0xBA) = v;
    if (*(int *)(s + 0x10) == 0)
        return;
    if (*(unsigned short *)(s + 0x3A) == 1)
        *(unsigned short *)(s + 0x3A) = 0;
    r4 = *(unsigned short *)(s + 0x8E);
    *(unsigned short *)(s + 0x42) ^= 1;
    p = *(unsigned char **)(s + *(unsigned short *)(s + 0x42) * 4 + 0x48);
    fn_02382DC0(a1 + 0x10, p, *(unsigned short *)(s + 0x46));
    r6 = fn_02382AC4();
    r5 = 0;
    if (*(unsigned short *)(s + 0x84) == 1) {
        r5 = 1;
        fn_0238219C(s + 0x58);
    }
    sb = 1;
    *(unsigned short *)(s + 0x84) = sb;
    *(unsigned short *)(s + 0x56) = *(unsigned short *)(p + 0xA);
    if (!(*(unsigned short *)(p + 0x2E) & (1 << *(unsigned short *)(s + 0x184))))
        sb = 0;
    *(unsigned short *)(s + 0x90) = sb;
    v = (unsigned short)(*(unsigned short *)(p + 0xA) - *(unsigned short *)(p + 0xC));
    v = (v + 0x60) << 4;
    v = v * 0x82EA;
    v = fn_0238D594(v, 0x40);
    v = fn_0238D594(v, 0x3E8);
    fn_023822A8(s + 0x58, v, v >> 31, fn_0238EFCC, 0);
    f = *(unsigned short *)(a1 + 0x18);
    b1 = (f & 0x2800) == 0x2800;
    b2 = (f & 0x6000) == 0x6000;
    if (b1)
        *(unsigned short *)(s + 0x8A) = 0;
    *(unsigned short *)(s + 0x8E) = b1 != 0;
    *(unsigned short *)(s + 0x8C) = b2 != 0;
    fn_02382AD8(r6);
    if (r5 != 0) {
        if (r4 == 1)
            fn_02393128(r5, 0);
        msg = (short *)fn_0238DE4C();
        msg[0] = 0xE;
        msg[1] = 9;
        msg[2] = 0xD;
        *(int *)(msg + 4) = 0;
        fn_0238DEF4((short *)msg);
    }
    fn_0238E57C(p + 0x18, a1 + 0x28);
    fn_0238E57C(p + 0x18, a1 + 0x2E);
    if (*(unsigned short *)(p + 6) >= 2) {
        *(unsigned short *)(p + 6) -= 2;
        *(unsigned short *)(s + 0x38) = (*(unsigned short *)(p + 0x30) & 0x8000) != 0;
        msg = (short *)fn_0238DE4C();
        msg[0] = 0xE;
        if (sb) msg[1] = 0;
        else msg[1] = 0xD;
        msg[2] = 0xC;
        *(unsigned char **)(msg + 4) = p;
        fn_0238DEF4((short *)msg);
        if (*(unsigned short *)(p + 6) != 0) {
            fn_02392F18(0, *(unsigned short *)(p + 0x30), p + 0x32, *(unsigned short *)(p + 6), p);
            fn_0238DEA8();
        }
    } else {
        *(unsigned short *)(p + 6) = 0;
        *(unsigned short *)(s + 0x38) = 0;
        msg = (short *)fn_0238DE4C();
        msg[0] = 0xE;
        if (sb) msg[1] = 0xE;
        else msg[1] = 0xD;
        msg[2] = 0xC;
        *(unsigned char **)(msg + 4) = p;
        fn_0238DEF4((short *)msg);
        fn_0238DEA8();
    }
}
