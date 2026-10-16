/* storage 0x0238E088 runtime 0x027C0000 size 0x34 main. Two BL relocs, no pool. */

extern int fn_0238DE4C(void);
extern int fn_0238DEF4(int);

int fn_0238E088(short a1, short a2, short a3) {
    short *p = (short *)fn_0238DE4C();
    p[0] = a1;
    p[1] = 1;
    p[2] = a2;
    p[3] = a3;
    return fn_0238DEF4((int)p);
}
