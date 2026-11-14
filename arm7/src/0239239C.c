extern void *fn_0238E688(void);
extern unsigned short *fn_0238FB00(void *buf);
extern int fn_02392360(int a1, int a2);
extern int fn_0238E2CC(int a1, void *buf);
extern unsigned short *fn_0238FB70(void *buf);
extern unsigned short *fn_0239045C(void *buf, int a1, int a2, int a3);
extern void fn_02382D2C(void *, int, int);
extern unsigned short *fn_02390034(void *buf, unsigned short a1, unsigned short a2, unsigned short a3, void *extra);
extern unsigned short *fn_0238FB88(void *buf);
extern unsigned short *fn_0238DE4C(void);
extern int fn_0238DEF4(int a1);

void fn_0239239C(unsigned short *arg) {
    char tmp[0x10];
    char buf[0x200];
    struct {
        int *slot;
        char *bufp;
    } pair;
    char *base;
    int *inner;
    unsigned short *res;
    unsigned short st;
    unsigned short mode;
    unsigned short *v;
    unsigned short lo;
    unsigned short hi;
    unsigned short mid;
    unsigned short top;

    base = (char *)fn_0238E688();
    base += 0x1000;
    pair.slot = *(int **)(base + 0x610);
    pair.bufp = buf;
    res = fn_0238FB00(pair.bufp);
    st = res[2];
    if (st != 0) {
        fn_02392360(0x308, st);
        return;
    }
    mode = res[3];
    inner = (int *)*pair.slot;
    ((unsigned short *)inner)[0x71] = 2;
    if (mode != 0x10)
        goto build;
    if (fn_0238E2CC(0xA, pair.bufp) == 0)
        return;
    res = fn_0238FB70(pair.bufp);
    st = res[2];
    if (st != 0) {
        fn_02392360(0x303, st);
        return;
    }
    inner = (int *)*pair.slot;
    *(unsigned short *)inner = 3;
    res = fn_0239045C(pair.bufp, 1, 0, 1);
    st = res[2];
    if (st != 0) {
        fn_02392360(1, st);
        return;
    }
build:
    lo = arg[1];
    hi = arg[2];
    mid = arg[3];
    top = arg[4];
    fn_02382D2C(tmp, 0, 0x10);
    tmp[0] = (char)mid;
    res = fn_02390034(pair.bufp, lo, hi, top, tmp);
    st = res[2];
    if (st != 0) {
        fn_02392360(0xA, st);
        return;
    }
    lo = (unsigned short)(res[4] & 0xFF);
    hi = (unsigned short)(res[4] >> 8);
    res = fn_0238FB88(pair.bufp);
    st = res[2];
    if (st != 0) {
        fn_02392360(0x302, st);
        return;
    }
    inner = (int *)*pair.slot;
    *(unsigned short *)inner = 2;
    v = fn_0238DE4C();
    v[0] = 0x1E;
    v[1] = 0;
    v[4] = lo;
    v[5] = hi;
    fn_0238DEF4((int)v);
}
