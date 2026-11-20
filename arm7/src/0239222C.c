extern char *fn_0238E688(void);
extern unsigned short *fn_0238FA8C(void *buf, int a1, int a2, int a3);
extern int fn_023921F0(int a1, int a2);
extern unsigned short *fn_0238DE4C(void);
extern int fn_0238DEF4(int a1);

void fn_0239222C(void) {
    struct {
        volatile int flag;
        char buf[0x204];
    } frame;
    char *base;
    int *obj;
    unsigned short *res;
    unsigned short st;
    unsigned short *v;
    base = fn_0238E688();
    base += 0x1000;
    obj = *(int **)(base + 0x610);
    frame.flag = 1;
    res = fn_0238FA8C(frame.buf, 0, 0, 0x14);
    st = res[2];
    if (st != 0) {
        fn_023921F0(0x309, st);
        return;
    }
    *(unsigned short *)*obj = 2;
    v = fn_0238DE4C();
    v[0] = 0x1B;
    v[1] = 0;
    fn_0238DEF4((int)v);
}
