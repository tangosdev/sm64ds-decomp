extern short *fn_0238DE4C(void);
extern int fn_0238DEF4(short *);

int fn_0238F5F8(void) {
    short *msg = fn_0238DE4C();
    msg[0] = 12;
    msg[1] = 0;
    msg[4] = 8;
    return fn_0238DEF4(msg);
}
