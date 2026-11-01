extern char *fn_0238E688(void);
extern int dat_27D80DC;
extern int fn_02381600(int a1, int a2, int a3);
extern unsigned short *fn_0238DE4C(void);
extern int fn_0238DEF4(int a1);

void fn_02392668(void) {
    char *base;
    unsigned short *v;
    base = fn_0238E688();
    dat_27D80DC = 28;
    if (fn_02381600((int)(base + 0x15C), (int)&dat_27D80DC, 0) == 0
        && *(int *)(base + 0x1000 + 0x610) != 0) {
        v = fn_0238DE4C();
        v[0] = 128;
        v[1] = 8;
        v[2] = 22;
        v[3] = 28;
        fn_0238DEF4((int)v);
    }
}
