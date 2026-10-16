extern void *fn_0238E688(void);
extern unsigned short *fn_0238E57C(unsigned short *result, unsigned short *a2);
extern void fn_023B2C80(unsigned short a1, void *a2, int a3);
extern void fn_023B2C98(void *a1, void *a2, int a3);
extern int fn_02381600(int a1, int a2, int a3);
extern void fn_023B2DC0(void *a1, void *a2, int a3);
extern unsigned short *fn_0238DE4C(void);
extern int fn_0238DEF4(int a1);
extern int dat_27D80D0;
extern int dat_27D80D4;

void fn_0238E9C8(unsigned char *a1) {
    unsigned char *v2;
    unsigned short *v5;
    int t;
    t = (int)fn_0238E688() + 0x1000;
    v2 = *(unsigned char **)*(int **)(t + 0x610);
    if (*(unsigned short *)(v2 + 0xF0) == 0) {
        unsigned short *v4;
        dat_27D80D0 = 0x22;
        fn_023B2C98(a1 + 0x10, &dat_27D80D4, 6);
        if (fn_02381600((int)fn_0238E688() + 0x15C, (int)&dat_27D80D0, 0) == 0) {
            v4 = fn_0238DE4C();
            v4[0] = 128;
            v4[1] = 8;
            v4[2] = 22;
            v4[3] = 34;
            fn_0238DEF4((int)v4);
        }
    } else {
        fn_0238E57C((unsigned short *)(v2 + 0x124 + 6 * (*(unsigned short *)(a1 + 0x16) - 1)), (unsigned short *)(a1 + 0x10));
        *(unsigned short *)(v2 + 0x17E) |= 1u << *(unsigned short *)(a1 + 0x16);
        {
            volatile unsigned short one = 1;
            fn_023B2C80(one, v2 + 0x1F4 + (*(unsigned short *)(a1 + 0x16) << 4), 0x20);
        }
        v5 = fn_0238DE4C();
        v5[0] = 8;
        v5[1] = 0;
        v5[4] = 7;
        fn_0238E57C(v5 + 5, (unsigned short *)(a1 + 0x10));
        v5[8] = *(unsigned short *)(a1 + 0x16);
        fn_023B2DC0(a1 + 0x22, v5 + 10, 0x18);
        fn_0238DEF4((int)v5);
    }
}
