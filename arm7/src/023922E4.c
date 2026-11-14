extern char *fn_0238E688(void);
extern unsigned short *fn_0238FD50(void *buf, unsigned short a1, unsigned short a2, unsigned short a3);
extern int fn_023922A8(int a1, int a2);
extern unsigned short *fn_0238DE4C(void);
extern int fn_0238DEF4(int a1);

void fn_023922E4(int *arg) {
    char buf[0x200];
    unsigned short *res;
    unsigned short st;
    unsigned short *v;
    fn_0238E688();
    res = fn_0238FD50(buf, (unsigned short)arg[1], (unsigned short)arg[2], (unsigned short)arg[3]);
    st = res[2];
    if (st != 0) {
        fn_023922A8(0x211, st);
        return;
    }
    v = fn_0238DE4C();
    v[0] = 0x1D;
    v[1] = 0;
    fn_0238DEF4((int)v);
}
