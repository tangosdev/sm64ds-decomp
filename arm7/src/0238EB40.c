extern short *fn_0238DE4C(void);
extern int fn_0238DEF4(short *);

int fn_0238EB40(void) {
    short *msg = fn_0238DE4C();
    msg[0] = 128;
    msg[1] = 0;
    msg[2] = 17;
    return fn_0238DEF4(msg);
}
