#pragma opt_propagation off

extern void *fn_0238E688(void);
extern void fn_023817E0(void *p);
extern void fn_02382C80(unsigned short value, void *dst, int n);
extern void fn_0238176C(void *p);

void fn_02393CA8(void) {
    volatile unsigned short zero;
    unsigned short i;
    unsigned short k;
    char *base = (char *)fn_0238E688();
    char *hi = base + 0x1000;
    char *obj = *(char **)(*(int *)(hi + 0x610));

    fn_023817E0(obj + 0x918);
    zero = 0;
    fn_02382C80(zero, obj + 0x2F4, 0x600);
    i = 0;
    do {
        unsigned short n = (unsigned short)(i + 1);
        *(unsigned short *)(obj + i * 0x18 + 0x2F4) = n;
        i = n;
    } while (i < 0x3F);
    *(unsigned short *)(obj + i * 0x18 + 0x2F4) = 0xFFFF;
    k = 0;
    *(unsigned short *)(obj + 0x8F4) = k;
    *(unsigned short *)(obj + 0x8F6) = i;
    do {
        *(unsigned short *)(obj + k * 4 + 0x908) = 0xFFFF;
        *(unsigned short *)(obj + k * 4 + 0x90A) = 0xFFFF;
        *(unsigned short *)(obj + k * 4 + 0x8F8) = 0xFFFF;
        *(unsigned short *)(obj + k * 4 + 0x8FA) = 0xFFFF;
        k = (unsigned short)(k + 1);
    } while (k < 4);
    fn_0238176C(obj + 0x918);
}
