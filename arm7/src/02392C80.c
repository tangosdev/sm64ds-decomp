/* storage 0x02392C80 runtime 0x027C4BF8 size 0x34 main. No pool. */

extern int *fn_0238E688(void);
extern void fn_02393C18(void);

void fn_02392C80(void) {
    int *base;
    unsigned short *obj;
    unsigned short mode;

    base = fn_0238E688();
    obj = *(unsigned short **)base[1412];
    mode = *obj;
    if (mode == 10)
        fn_02393C18();
}
