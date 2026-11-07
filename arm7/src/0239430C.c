extern void *fn_0238E688(void);
extern unsigned short *fn_0238FBA0(unsigned short *msg);
extern unsigned short *fn_0238DE4C(void);
extern int fn_0238DEF4(unsigned short *msg);

void fn_0239430C(void) {
    unsigned short buf[0x100];
    char *base = (char *)fn_0238E688();
    char *hi = base + 0x1000;
    unsigned short **pp = *(unsigned short ***)(hi + 0x610);
    if (**pp != 2) {
        unsigned short *msg = fn_0238DE4C();
        msg[0] = 6;
        msg[1] = 3;
        fn_0238DEF4(msg);
    } else {
        unsigned short *ret = fn_0238FBA0(buf);
        if (ret[2] != 0) {
            unsigned short *msg = fn_0238DE4C();
            msg[0] = 6;
            msg[1] = 1;
            msg[2] = 0x301;
            msg[3] = ret[2];
            fn_0238DEF4(msg);
        } else {
            **pp = 1;
            unsigned short *msg = fn_0238DE4C();
            msg[0] = 6;
            msg[1] = 0;
            fn_0238DEF4(msg);
        }
    }
}
