extern int fn_0238E688(void);
extern void fn_0238E57C(void *, void *);
extern void fn_02390214(void *, void *, int);
extern unsigned short *fn_023904C0(void *, int);
extern int fn_02390C28(int, int);
extern unsigned short *fn_0238FB88(void *);
extern unsigned short *fn_0238DE4C(void);
extern int fn_0238DEF4(unsigned short *);

void fn_02390C64(void)
{
    unsigned char buf0[8];
    unsigned char buf8[0x200];
    int *v10;
    int i;
    unsigned int bit;
    unsigned short *p;
    unsigned short *v3;
    int v1;
    int v0 = fn_0238E688() + 0x1000;
    v10 = *(int **)(v0 + 0x610);
    for (i = 1; i < 0x10; i++) {
        v3 = (unsigned short *)*v10;
        bit = 1 << i;
        if (v3[0xBF] & bit) {
            fn_0238E57C(buf0, (void *)(*v10 + 0x124 + (i - 1) * 6));
            fn_02390214(buf8, buf0, 3);
            bit = ~bit;
            *(unsigned short *)(*v10 + 0x17E) &= bit;
            *(unsigned short *)(*v10 + 0x86) &= bit;
        }
    }
    p = fn_023904C0(buf8, 1);
    v1 = p[2];
    if (v1 != 0) {
        fn_02390C28(0, v1);
    } else {
        p = fn_0238FB88(buf8);
        v1 = p[2];
        if (v1 != 0) {
            fn_02390C28(0x302, v1);
        } else {
            v3 = (unsigned short *)*v10;
            v3[0] = 2;
            v1 = 0;
            ((unsigned short *)*v10)[0xC7] = v1;
            ((unsigned short *)*v10)[0xC8] = v1;
            p = fn_0238DE4C();
            p[0] = 9;
            p[1] = 0;
            fn_0238DEF4(p);
        }
    }
}
