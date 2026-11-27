/* storage 0x02392CB4 runtime 0x027C4C2C size 0x3C main. No pool. */

extern int *fn_0238E688(void);
extern void fn_02393A90(int);

void fn_02392CB4(int *arg) {
    int *base;
    unsigned short *obj;
    unsigned short mode;
    unsigned int value;

    base = fn_0238E688();
    obj = *(unsigned short **)base[1412];
    mode = *obj;
    if (mode == 9) {
        value = (unsigned int)(arg[1] << 16);
        value >>= 16;
        fn_02393A90((int)value);
    }
}
