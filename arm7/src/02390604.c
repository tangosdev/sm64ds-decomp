extern void *fn_0238E688(void);
extern unsigned short *fn_0238FB00(unsigned short *a1);
extern unsigned short *fn_0238FBB8(unsigned short *a1);
extern unsigned short *fn_0238E57C(unsigned short *result, unsigned short *a2);
extern unsigned short *fn_02390214(unsigned short *a1, void *src, unsigned short a3);
extern unsigned short *fn_023904C0(unsigned short *a1, unsigned short a2);
extern unsigned short *fn_0238FB88(unsigned short *a1);
extern unsigned short *fn_0238FA8C(unsigned short *a1, unsigned short a2, unsigned short a3,
                                   unsigned short a4, unsigned short a5);
extern int fn_023905CC(int a1, int a2);
extern unsigned short *fn_0238DE4C(void);
extern int fn_0238DEF4(unsigned short *msg);

int fn_02390604(void) {
#pragma opt_propagation off
    unsigned short scratch[4];
    unsigned short buf[256];
    char *base;
    char *hi;
    volatile int **pp;
    unsigned short *p;
    unsigned short *ret;
    unsigned short st;
    int kind;
    int tag;

    base = (char *)fn_0238E688();
    hi = base + 0x1000;
    pp = *(volatile int ***)(hi + 0x610);
    p = buf;
    (*pp)[4] = 0;
    (*pp)[6] = 0;
    (*pp)[5] = 0;

    ret = fn_0238FB00(p);
    st = ret[2];
    if (st != 0)
        return fn_023905CC(0x308, st);
    kind = ret[3];

    ret = fn_0238FBB8(p);
    st = ret[2];
    if (st != 0)
        return fn_023905CC(0x308, st);
    tag = ret[3];

    switch (kind) {
    case 0x40:
    case 0x30:
        if ((unsigned short)(tag + 0xFFFE) <= 1) {
            fn_0238E57C(scratch, (unsigned short *)((char *)*pp + 0x186));
            ret = fn_02390214(p, scratch, 3);
            st = ret[2];
            if (st != 0)
                return fn_023905CC(5, st);
        }
    case 0x20:
        ret = fn_023904C0(p, 1);
        st = ret[2];
        if (st != 0)
            return fn_023905CC(0, st);
    case 0:
        ret = fn_0238FB88(p);
        st = ret[2];
        if (st != 0)
            return fn_023905CC(0x302, st);
    case 0x10:
        if (tag == 0) {
            ret = fn_0238FA8C(p, 0, 0, 0x14, 1);
            st = ret[2];
            if (st != 0)
                return fn_023905CC(0x309, st);
        }
    default:
        *(volatile unsigned short *)*pp = 2;
        {
            unsigned short *msg = fn_0238DE4C();
            msg[0] = 1;
            msg[1] = 0;
            return fn_0238DEF4(msg);
        }
    }
}
