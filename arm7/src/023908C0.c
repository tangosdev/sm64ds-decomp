extern short *fn_0238DE4C(void);
extern int fn_0238DEF4(short *msg);

int fn_023908C0(int a1, int a2) {
    short *msg = fn_0238DE4C();
    msg[0] = 7;
    msg[1] = 1;
    msg[2] = a1;
    msg[3] = a2;
    return fn_0238DEF4(msg);
}
