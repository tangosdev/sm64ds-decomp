extern int fn_0238E688(void);
extern unsigned short *fn_0238DE4C(void);
extern int fn_0238DEF4(unsigned short *);
extern unsigned short *fn_0238FB00(void *);
extern int fn_02390D9C(int, int);
extern int fn_0238E2CC(int, void *);
extern unsigned short *fn_0238FB70(void *);
extern unsigned short *fn_0239045C(void *, int, int, int);
extern void fn_02382C80(int, void *, int);
extern void fn_02382D2C(void *, int, int);
extern unsigned short *fn_023903A0(void *, int, void *, int, void *, int, void *, int);
extern void fn_02382DC0(void *, void *, unsigned int);
extern int fn_0238E184(int);
extern int fn_0238E0BC(int);
extern void fn_0238E57C(void *, void *);
extern void fn_02382C98(void *, void *, int);

void fn_02390DD8(unsigned short *a1) {
    volatile unsigned short zffff;
    volatile unsigned short z0;
    volatile unsigned short z1;
    unsigned short hdr[3];
    unsigned char name[0x20];
    unsigned char id[0x10];
    unsigned char big[0x200];
    unsigned char *buf;
    int base;
    int *rec;
    unsigned short idx;
    unsigned short hw;
    unsigned short *p;
    unsigned short st;
    unsigned short *msg;

    buf = big;
    base = fn_0238E688() + 0x1000;
    rec = *(int **)(base + 0x610);
    *(int *)(*rec + 0x180) = *(int *)((char *)a1 + 4);
    idx = a1[1];
    {
        char *page = (char *)*rec + 0x100;
        *(unsigned short *)(page + 0x8C) = idx;
    }
    hw = *(unsigned short *)((char *)a1 + 8);
    hdr[0] = *(unsigned short *)((char *)a1 + 0xA);
    hdr[1] = *(unsigned short *)((char *)a1 + 0xC);
    hdr[2] = *(unsigned short *)((char *)a1 + 0xE);
    if (idx == 0) {
        p = fn_0238DE4C();
        p[0] = 10;
        p[1] = 6;
        fn_0238DEF4(p);
    }
    {
        unsigned mask = 1u << idx;
        char *page = (char *)*rec + 0x100;
        if ((mask & *(unsigned short *)(page + 0xF0)) == 0) {
            p = fn_0238DE4C();
            p[0] = 10;
            p[1] = 6;
            fn_0238DEF4(p);
            return;
        }
    }
    *(unsigned short *)(*rec + 0xE2) = 2;
    p = fn_0238FB00(buf);
    st = p[2];
    if (st != 0) {
        fn_02390D9C(0x308, st);
        return;
    }
    if (p[3] == 0x10) {
        if (fn_0238E2CC(10, buf) == 0)
            return;
        p = fn_0238FB70(buf);
        st = p[2];
        if (st != 0) {
            fn_02390D9C(0x303, st);
            return;
        }
        *(unsigned short *)*rec = 3;
        p = fn_0239045C(buf, 1, 0, 1);
        st = p[2];
        if (st != 0) {
            fn_02390D9C(1, st);
            return;
        }
    }
    *(unsigned short *)*rec = 5;
    zffff = (unsigned short)0xFFFF;
    fn_02382C80(zffff, name, 0x20);
    id[0] = (unsigned char)idx;
    fn_02382D2C(id + 1, 0, 0xF);
    p = fn_023903A0(buf, 0x11C, hdr, 0, name, 1, id, hw);
    st = p[2];
    if (st != 0) {
        fn_02390D9C(2, st);
        return;
    }
    msg = fn_0238DE4C();
    if (p[4] == 0) {
        msg[0] = 10;
        msg[1] = 0;
        msg[4] = 4;
        msg[8] = idx;
        msg[9] = 0;
    } else {
        z0 = 0;
        fn_02382C80(z0, (char *)*(int *)(*rec + 0x180) + 0x40, 0x80);
        fn_02382DC0((char *)p + 0xA, *(void **)(*rec + 0x180), 0xC0);
        msg[0] = 10;
        msg[1] = 0;
        msg[4] = 5;
        msg[8] = *(unsigned short *)((char *)p + 0x40);
        msg[9] = (unsigned short)fn_0238E0BC(fn_0238E184((unsigned char)p[6]));
        msg[10] = *(unsigned short *)((char *)p + 0x14);
        fn_0238E57C(msg + 5, (unsigned short *)((char *)p + 0xE));
        fn_02382C98((char *)p + 0x16, (char *)msg + 0x16, 0x20);
        *(unsigned short *)((char *)msg + 0x36) = *(unsigned short *)((char *)p + 0x46);
        if (*(unsigned short *)((char *)msg + 0x36) > 0x80) {
            msg[0] = 10;
            msg[1] = 0;
            msg[4] = 4;
            msg[8] = idx;
            msg[9] = 0;
        } else {
            z1 = 0;
            fn_02382C80(z1, (char *)msg + 0x38, 0x80);
            fn_02382C98((char *)p + 0x4A, (char *)msg + 0x38,
                        (*(unsigned short *)((char *)msg + 0x36) + 1) & ~1);
        }
    }
    fn_0238DEF4(msg);
}
