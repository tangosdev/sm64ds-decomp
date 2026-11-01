extern unsigned char dat_27D6A84[];
extern int fn_02381600(int a1, int a2, int a3);
extern int fn_0238155C(int a1, int **a2, int a3);
extern unsigned short *fn_0238DE4C(void);
extern int fn_0238DEF4(int a1);
extern void fn_023833CC(void);
extern void fn_02382B38(void);

int fn_0238E610(int a1) {
    int *slot;
    unsigned short *v0;
    unsigned short index;
    fn_02381600((int)dat_27D6A84, a1, 1);
    fn_0238155C(0x27D6B4C, &slot, 1);
    index = *(unsigned short *)((char *)slot + 0xE);
    if (*(unsigned short *)((char *)slot + (index << 1) + 0x14) == 14) {
        v0 = fn_0238DE4C();
        v0[0] = 128;
        v0[1] = 19;
        fn_0238DEF4((int)v0);
        fn_023833CC();
        fn_02382B38();
    }
    return (int)slot;
}
