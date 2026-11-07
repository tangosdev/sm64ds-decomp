extern short *fn_0238DE4C(void);
extern int fn_0238DEF4(short *);

int fn_0238F740(void) {
    short *msg = fn_0238DE4C();
    msg[0] = 8;
    msg[1] = 0;
    msg[4] = 2;
    return fn_0238DEF4(msg);
}
