/* storage 0x02391200 runtime 0x027C3178 linked 0x45C main.
 * Parent record at fn_0238E688()+0x1610. Period and flag checks, then
 * three request round-trips. The 64-bit-looking constants 0x1FFF, 0x303
 * and 0x186 are plain integer pools. */
extern void *fn_0238E688(void);
extern void fn_02382DC0(void *src, void *dst, unsigned int n);
extern unsigned short *fn_0238DE4C(void);
extern int fn_0238DEF4(unsigned short *p);
extern int fn_0238E2CC(int a, unsigned short *b);
extern unsigned short *fn_0238FB70(unsigned short *a);
extern int fn_023911C4(int a, int b);
extern unsigned short *fn_0239031C(unsigned short *a, unsigned short b, void *src);
extern void fn_0238E57C(void *dst, void *src);
extern unsigned short *fn_02390298(unsigned short *a, void *src, int c, int d);
extern unsigned short *fn_02390194(unsigned short *a, void *src, int c, int d);
extern void fn_02382C80(int v, void *dst, int n);
extern void fn_02393CA8(void);
extern int fn_0238E184(int a);
extern int fn_0238E0BC(int a);
extern int fn_0238E120(int a);

struct Big {
    int w;
    unsigned short rest[0xF6];
};

void fn_02391200(int *a1) {
    volatile unsigned short one;
    unsigned short tmp2[3];
    unsigned short tmp8[3];
    struct Big big;
    unsigned short area[0x2C];
    unsigned short *bp;
    char *base;
    unsigned short *rec;
    char *obj;
    unsigned short *p;
    unsigned short *res;
    int bit;
    int saved;

    bp = (unsigned short *)&big;
    base = (char *)fn_0238E688();
    rec = *(unsigned short **)(base + 0x1610);
    fn_02382DC0(*(void **)((char *)a1 + 4), rec + 8, 0xC0);
    if (rec[0x26] >= 0x10 && (((unsigned char *)rec)[0x5B] & 1) == 0) {
        p = fn_0238DE4C();
        p[0] = 12;
        p[1] = 11;
        p[4] = 6;
        fn_0238DEF4(p);
        return;
    }

    bit = 1 << rec[0x23];
    if ((bit & *(unsigned short *)(*(char **)rec + 0x1F0)) == 0
        || ((bit >> 1) & 0x1FFF) == 0) {
        p = fn_0238DE4C();
        p[0] = 12;
        p[1] = 6;
        p[4] = 6;
        fn_0238DEF4(p);
        return;
    }

    p = fn_0238DE4C();
    p[0] = 12;
    p[1] = 0;
    p[4] = 6;
    fn_0238DEF4(p);

    obj = *(char **)rec;
    obj += 0x100;
    if (*(unsigned short *)(obj + 0xE8) == 1) {
        if (rec[0x1F] & 1)
            *(unsigned short *)(obj + 0xE8) = 1;
        else
            *(unsigned short *)(obj + 0xE8) = 2;
    } else if (rec[0x1F] & 2) {
        *(unsigned short *)(obj + 0xE8) = 2;
    } else {
        *(unsigned short *)(obj + 0xE8) = 1;
    }

    if (rec[0x1E] & 0x20)
        *(unsigned short *)(*(char **)rec + 0x1EA) = 1;
    else
        *(unsigned short *)(*(char **)rec + 0x1EA) = 0;

    if (rec[0x1E] & 0x10)
        *(int *)(*(char **)rec + 0x194) = 1;
    else
        *(int *)(*(char **)rec + 0x194) = 0;

    if (rec[0x26] == 0)
        *(unsigned short *)(*(char **)rec + 0xE2) = 3;
    else
        *(unsigned short *)(*(char **)rec + 0xE2) = 2;

    if (fn_0238E2CC(12, bp) == 0)
        return;

    res = fn_0238FB70(bp);
    if (res[2] != 0) {
        fn_023911C4(0x303, res[2]);
        return;
    }

    fn_02382DC0(rec + 8, area + 8, 0x40);
    if (*(unsigned short *)(*(char **)rec + 0xE2) == 2) {
        area[13] = 0x20;
        area[14] = rec[0x2A];
        area[15] = rec[0x2B];
        area[16] = rec[0x2C];
        area[17] = 0;
        fn_02382DC0((char *)a1 + 8, area + 18, 0x18);
    }

    res = fn_0239031C(bp, 0x7D0, area + 8);
    if (res[2] != 0 || res[3] != 0) {
        fn_023911C4(3, res[2]);
        return;
    }

    fn_0238E57C(*(char **)rec + 0x186, res + 4);
    {
        int add = (((unsigned char *)rec)[0x5B] & 4) ? 6 : 0;
        *(unsigned short *)(*(char **)rec + 0x18E) = (unsigned short)(rec[0x2F] + 2 + add);
        add = (((unsigned char *)rec)[0x5B] & 4) ? 0x2A : 0;
        *(unsigned short *)(*(char **)rec + 0x190) = (unsigned short)(rec[0x2E] + 4 + add);
    }

    fn_0238E57C(tmp2, *(char **)rec + 0x186);
    res = fn_02390298(bp, tmp2, 0, 0x7D0);
    if (res[2] == 12 && res[3] == 19) {
        p = fn_0238DE4C();
        p[0] = 12;
        p[1] = 12;
        fn_0238DEF4(p);
        return;
    }
    if (res[2] != 0 || res[3] != 0) {
        fn_023911C4(4, res[2]);
        return;
    }

    fn_0238E57C(tmp8, *(char **)rec + 0x186);
    res = fn_02390194(bp, tmp8, 1, 0x7D0);
    if (res[2] == 12 && res[3] == 19) {
        p = fn_0238DE4C();
        p[0] = 12;
        p[1] = 11;
        fn_0238DEF4(p);
        return;
    }
    if (res[2] != 0 || res[3] != 0) {
        fn_023911C4(6, res[2]);
        return;
    }

    *(unsigned short *)(*(char **)rec + 0x184) = res[4];
    *(unsigned short *)(*(char **)rec + 0xB6) = rec[0x2C];
    one = 1;
    fn_02382C80(one, *(char **)rec + 0x1F4, 0x20);
    fn_02393CA8();
    saved = fn_0238E184((unsigned char)rec[9]);
    *(unsigned short *)(*(char **)rec + 0xB8) = (unsigned short)fn_0238E0BC(saved);
    fn_0238E120(saved);
    **(unsigned short **)rec = 8;
    p = fn_0238DE4C();
    p[0] = 12;
    p[1] = 0;
    p[4] = 7;
    p[5] = *(unsigned short *)(*(char **)rec + 0x184);
    fn_0238DEF4(p);
}
