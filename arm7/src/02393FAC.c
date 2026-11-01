extern void *fn_0238E688(void);
extern void fn_02389D60(int code);
extern unsigned short *fn_0238DE4C(void);
extern int fn_0238DEF4(unsigned short *msg);

void fn_02393FAC(void) {
    char *base = (char *)fn_0238E688();
    char *hi = base + 0x1000;
    unsigned short **pp = *(unsigned short ***)(hi + 0x610);
    if (**pp != 1) {
        unsigned short *msg = fn_0238DE4C();
        msg[0] = 4;
        msg[1] = 3;
        fn_0238DEF4(msg);
    } else {
        fn_02389D60(1);
        **pp = 0;
        unsigned short *msg = fn_0238DE4C();
        msg[0] = 4;
        msg[1] = 0;
        fn_0238DEF4(msg);
    }
}
