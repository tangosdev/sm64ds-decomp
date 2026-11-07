extern char *fn_0238E688(void);
extern unsigned short *fn_0238FB14(void *buf);
extern int fn_023925BC(int a1, int a2);
extern unsigned short *fn_0238DE4C(void);
extern int fn_0238DEF4(int a1);
extern void fn_02382C98(void *a1, void *a2, int n);

void fn_023925F8(void) {
    char buf[0x200];
    unsigned short *res;
    unsigned short st;
    unsigned short *v;
    fn_0238E688();
    res = fn_0238FB14(buf);
    st = res[2];
    if (st != 0) {
        fn_023925BC(0x307, st);
        return;
    }
    v = fn_0238DE4C();
    v[0] = 0x20;
    v[1] = 0;
    fn_02382C98(res + 4, v + 4, 0xB4);
    fn_0238DEF4((int)v);
}
