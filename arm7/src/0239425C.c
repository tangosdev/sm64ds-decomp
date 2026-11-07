extern void *fn_0238E688(void);
extern int fn_02394054(unsigned short *a, unsigned short *b);
extern unsigned short *fn_0238DE4C(void);
extern int fn_0238DEF4(unsigned short *msg);

void fn_0239425C(void) {
    unsigned short tmp[2];
    char *base = (char *)fn_0238E688();
    char *hi = base + 0x1000;
    unsigned short **pp = *(unsigned short ***)(hi + 0x610);
    if (**pp != 1) {
        unsigned short *msg = fn_0238DE4C();
        msg[0] = 5;
        msg[1] = 3;
        fn_0238DEF4(msg);
    } else if (fn_02394054(&tmp[0], &tmp[1]) == 0) {
        unsigned short *msg = fn_0238DE4C();
        msg[0] = 5;
        msg[1] = 1;
        msg[2] = tmp[0];
        msg[3] = tmp[1];
        fn_0238DEF4(msg);
    } else {
        **pp = 2;
        unsigned short *msg = fn_0238DE4C();
        msg[0] = 5;
        msg[1] = 0;
        fn_0238DEF4(msg);
    }
}
