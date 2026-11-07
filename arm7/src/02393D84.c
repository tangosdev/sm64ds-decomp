extern void *fn_0238E688(void);
extern void fn_0238E448(void *dst, void *src);
extern unsigned short *fn_0238FBFC(void *a, unsigned short b, void *c);
extern unsigned short *fn_0238DE4C(void);
extern int fn_0238DEF4(unsigned short *msg);

void fn_02393D84(int *a1) {
    unsigned char lower[0x200];
    unsigned char upper[0x84];
    char *base = (char *)fn_0238E688();
    char *hi = base + 0x1000;
    char *obj = *(char **)(*(int *)(hi + 0x610));
    *(unsigned short *)(obj + 0xF0) = a1[1];
    fn_0238E448(upper, obj + 0xE4);
    unsigned short n = *(unsigned short *)(obj + 0xE8);
    unsigned short *ret = fn_0238FBFC(lower, (unsigned short)(n + 0x10), upper);
    unsigned short *msg = fn_0238DE4C();
    msg[0] = 0x21;
    if (ret[2] == 0) {
        msg[1] = 0;
    } else {
        msg[1] = 1;
        msg[2] = 0x245;
        msg[3] = ret[2];
    }
    fn_0238DEF4(msg);
}
