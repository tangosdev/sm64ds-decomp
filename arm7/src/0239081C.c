extern void *fn_0238E688(void);
extern unsigned short *fn_0238DE4C(void);
extern int fn_0238DEF4(unsigned short *msg);
extern unsigned short *fn_0238FBA0(unsigned short *msg);
extern int fn_023907E0(int a1, int a2);
extern void fn_02389D60(int code);

void fn_0239081C(void) {
    unsigned short buf[0x100];
    char *base = (char *)fn_0238E688();
    char *hi = base + 0x1000;
    unsigned short **pp = *(unsigned short ***)(hi + 0x610);
    if (**pp != 2) {
        unsigned short *msg = fn_0238DE4C();
        msg[0] = 2;
        msg[1] = 3;
        fn_0238DEF4(msg);
    } else {
        unsigned short *ret = fn_0238FBA0(buf);
        unsigned short h = ret[2];
        if (h != 0) {
            fn_023907E0(0x301, h);
        } else {
            **pp = 1;
            fn_02389D60(1);
            **pp = 0;
            unsigned short *msg = fn_0238DE4C();
            msg[0] = 2;
            msg[1] = 0;
            fn_0238DEF4(msg);
        }
    }
}
