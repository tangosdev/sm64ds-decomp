extern int fn_0238E688(void);
extern unsigned short *fn_0238DE4C(void);
extern int fn_0238DEF4(unsigned short *);
extern int fn_0238E2CC(int, void *);
extern unsigned short *fn_0238FB70(void *);
extern unsigned short *fn_0239045C(void *, int, int, int);
extern int fn_023909B4(int, int);
extern void fn_02382C80(int, void *, int);
extern void fn_0238E448(void *, void *);
extern unsigned short *fn_023900C0(void *, int, void *, int, int, int, int, int, int, void *);

void fn_023909F8(void)
{
    volatile unsigned short a;
    volatile unsigned short b;
    unsigned short s[0x10];
    unsigned char big[0x200];
    unsigned char b84[0x80];
    volatile int *v6;
    unsigned char *v5 = big;
    unsigned short *v4;
    unsigned short *v3;
    unsigned short *p;
    int v1;
    int v0 = fn_0238E688() + 0x1000;
    v6 = *(int **)(v0 + 0x610);
    v3 = (unsigned short *)*v6;
    if (((int)(v3[0xF9] & ((1 << v3[0x8A]) >> 1))) == 0) {
        p = fn_0238DE4C();
        p[0] = 8;
        p[1] = 6;
        fn_0238DEF4(p);
        return;
    }
    v3[0x71] = 1;
    v1 = 0;
    ((unsigned short *)*v6)[0xC2] = v1;
    ((unsigned short *)*v6)[0xBF] = v1;
    ((unsigned short *)*v6)[0x43] = v1;
    if (fn_0238E2CC(8, v5) == 0) {
        return;
    }
    p = fn_0238FB70(v5);
    v1 = p[2];
    if (v1 != 0) {
        fn_023909B4(0x303, v1);
        return;
    }
    p = fn_0239045C(v5, 1, 0, 1);
    v1 = p[2];
    if (v1 != 0) {
        fn_023909B4(1, v1);
        return;
    }
    v4 = (unsigned short *)(*v6 + 0xE4);
    a = 0;
    fn_02382C80(a, b84, 0x80);
    fn_0238E448(b84, v4);
    b = 0;
    fn_02382C80(b, s, 0x20);
    s[0] = v4[3];
    s[1] = v4[4];
    s[2] = v4[5];
    s[3] = 0;
    p = fn_023900C0(v5, 0x20, s, v4[0xB], 2, v4[0x18], 3, 3,
                    (unsigned short)(v4[2] + 0x10), b84);
    v1 = p[2];
    if (v1 != 0) {
        fn_023909B4(9, v1);
        return;
    }
    ((unsigned short *)*v6)[0xC7] = v4[0x19] + 4 + (v4[9] ? 0x2A : 0);
    ((unsigned short *)*v6)[0xC8] = v4[0x1A] + 2 + (v4[9] ? 6 : 0);
    p = fn_0238DE4C();
    ((unsigned short *)*v6)[0] = 7;
    p[0] = 8;
    p[1] = 0;
    p[4] = 0;
    fn_0238DEF4(p);
}
