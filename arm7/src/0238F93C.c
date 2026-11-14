/* storage 0x0238F93C runtime 0x027C18B4 size 0x7C main. */

extern int *fn_0238E688(void);
extern void fn_02381C5C(void *a1, void *a2, int a3);
extern void fn_0238F7B4(int a1);
extern int fn_02381D00(void *a1, void *a2, int a3);

void fn_0238F93C(void) {
    int *base;
    char *r7;
    int one;
    char *v1;
    unsigned short v0;
    unsigned short lo;
    unsigned short hi;

    base = fn_0238E688();
    r7 = (char *)base + 0xC8;
    one = 1;
    while (1) {
        char *r6 = (char *)base + 0x34;
        fn_02381C5C(r6, &v1, one);
        v0 = *(unsigned short *)(v1 + 12);
        lo = (unsigned short)(v0 & 0xFF);
        hi = (unsigned short)(v0 & 0xFF00);
        if ((hi == 0 || hi == 0x100) && (lo & 0x80) != 0)
            fn_0238F7B4((int)v1);
        else
            fn_02381D00(r7, v1, one);
    }
}
