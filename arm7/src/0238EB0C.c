extern short *fn_0238DE4C(void);
extern int fn_0238DEF4(short *);

int fn_0238EB0C(void) {
    short *msg = fn_0238DE4C();
    msg[0] = 128;
    msg[1] = 0;
    msg[2] = 18;
    return fn_0238DEF4(msg);
}
