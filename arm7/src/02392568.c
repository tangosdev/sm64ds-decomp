extern char *fn_0238E688(void);
extern unsigned short *fn_0238FB44(void *buf);
extern int fn_0239252C(int a1, int a2);
extern unsigned short *fn_0238DE4C(void);
extern int fn_0238DEF4(int a1);

void fn_02392568(void) {
    char buf[0x200];
    unsigned short *res;
    unsigned short st;
    unsigned short *v;
    fn_0238E688();
    res = fn_0238FB44(buf);
    st = res[2];
    if (st != 0) {
        fn_0239252C(0x305, st);
        return;
    }
    v = fn_0238DE4C();
    v[0] = 0x1F;
    v[1] = 0;
    fn_0238DEF4((int)v);
}
