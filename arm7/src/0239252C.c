extern unsigned short *fn_0238DE4C(void);
extern int fn_0238DEF4(int a1);

int fn_0239252C(int a1, int a2) {
    unsigned short *v;
    v = fn_0238DE4C();
    v[0] = 0x1F;
    v[1] = 1;
    v[2] = (unsigned short)a1;
    v[3] = (unsigned short)a2;
    return fn_0238DEF4((int)v);
}
