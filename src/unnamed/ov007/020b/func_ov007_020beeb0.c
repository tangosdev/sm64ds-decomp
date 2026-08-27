#pragma opt_strength_reduction off
typedef long long s64;
typedef unsigned char u8;
typedef unsigned short u16;

extern int func_ov007_020c1da0(int i);
extern int func_020531a4(int a);
extern int _ZN4cstd3divEii(int a, int b);
extern void func_ov007_020bed60(int a, int b, int c);
extern char *func_ov007_020b2cdc(void);
extern void func_ov007_020ca1d4(char *self, int a);
extern void func_ov007_020cba78(char *c, int a);
extern void func_ov007_020bf57c(char *a, int idx);
extern void func_ov007_020c9c24(char *a, int b);
extern void func_ov007_020c9a84(char *a, char *b, char *c, char *d);
extern void func_ov007_020bce70(int n, int m);
extern unsigned short func_ov007_020c3be0(int t, int a, int b);

extern u8 data_ov007_0210342c[];
extern u8 data_ov007_02104bd4[];

void func_ov007_020beeb0(void)
{
    int sp0;
    int sp4;
    int sp8;
    int r4;

    r4 = *(int *)(*(char **)data_ov007_02104bd4);

    if (*(u16 *)(*(char **)(*(char **)data_ov007_0210342c + 0x50) + 0xc) != 0) {
        int r6;
        char *base;
        if (func_ov007_020c1da0(0) == 0)
            r6 = 0x1000;
        else
            r6 = 0xbb8;
        base = *(char **)(*(char **)data_ov007_0210342c + 0x50);
        if (*(int *)(base + 0x20) != 0) {
            int s2 = *(short *)(base + 0x3e);
            int s1 = *(short *)(base + 0x3c);
            int d = _ZN4cstd3divEii(func_020531a4((s1 * s1 + s2 * s2) << 0xc), 0x1e);
            int r2 = (int)(((s64)r6 * d + 0x800) >> 0xc);
            if (r2 >= 0x1000)
                r2 = 0x1000;
            r6 = (int)(((s64)r2 * r2) >> 0xc);
        }
        base = *(char **)(*(char **)data_ov007_0210342c + 0x50);
        func_ov007_020bed60(*(u16 *)(base + 8), *(u16 *)(base + 0xa), r6);
    } else {
        char *p = func_ov007_020b2cdc();
        int a;
        if (*(int *)(p + 0xc) == 0) {
            a = *(short *)p;
            if (a >= 0 && a < 0x18) {
                a = *(short *)(p + 0xa);
                if (a >= 0 && a < 0x18) {
                    char *q = *(char **)(*(char **)data_ov007_0210342c + a * 4 + 0x114);
                    char *v = *(char **)q;
                    func_ov007_020bed60(*(int *)(v + 8) >> 0xc, *(int *)(v + 0xc) >> 0xc, 0x7d0);
                }
            }
        }
    }

    func_ov007_020ca1d4(*(char **)(*(char **)data_ov007_02104bd4 + 0xc), r4);
    func_ov007_020cba78((char *)r4, 2);

    for (int i = 0; i < 8; i++) {
        char *r8p = *(char **)(*(char **)data_ov007_02104bd4 + i * 4 + 0x10);
        char *r6p = *(char **)(*(char **)data_ov007_0210342c + (i + 0xe) * 4 + 0x114);
        char *w;
        int t;
        if (*(int *)(*(char **)(*(char **)r6p + 4) + 4) == 2)
            func_ov007_020bf57c(*(char **)(*(char **)data_ov007_0210342c + 0x30), i);
        if (*(int *)(*(char **)r6p + 0x20) != 0) {
            func_ov007_020c9c24(r8p, 2);
            w = *(char **)(r6p + 4);
            func_ov007_020c9a84(r8p, w + 0x28, w + 0x34, w + 0x36);
            w = *(char **)(r6p + 4);
            t = *(int *)(w + 0x28);
            *(int *)(w + 0x28) = (int)(((s64)t * 0x64 + 0x800) >> 0xc);
            w = *(char **)(r6p + 4);
            t = *(int *)(w + 0x2c);
            *(int *)(w + 0x2c) = (int)(((s64)t * 0x64 + 0x800) >> 0xc);
        }
    }

    int r5;
    switch (*(short *)(*(char **)(*(char **)data_ov007_0210342c + 8))) {
    case 0:
    case 1:
    case 11:
    case 12:
        sp0 = 4;
        r5 = 0x1e;
        sp4 = 0x7fff;
        sp8 = 0x4800;
        break;
    case 2:
    case 3:
    case 4:
    case 5:
    case 6:
        sp0 = 5;
        r5 = 0x3c;
        sp4 = 0x575c;
        sp8 = 0x178;
        break;
    case 7:
    case 8:
        sp0 = 6;
        r5 = 0x3c;
        sp4 = 0x631a;
        sp8 = 0x7fbf;
        break;
    case 9:
    case 10:
        r5 = 0x3c;
        sp4 = 0x7fff;
        sp8 = 0x49e0;
        sp0 = 7;
        break;
    default:
        break;
    }

    func_ov007_020bce70(sp0, r5);

    {
        char *o8 = *(char **)(*(char **)data_ov007_02104bd4 + 8);
        int n = *(int *)(*(char **)(*(char **)data_ov007_0210342c + 0x40) + 0xc);
        u16 *p7 = (u16 *)(o8 + 0x28);
        u16 *p6 = (u16 *)(o8 + 0x2a);
        int ratio;
        if (n <= 0)
            ratio = 0;
        else if (n >= r5)
            ratio = 0x1000;
        else
            ratio = _ZN4cstd3divEii(n << 0xc, r5);
        *p7 = func_ov007_020c3be0(ratio, *p7, sp4);
        *p6 = func_ov007_020c3be0(ratio, *p6, sp8);
    }

    {
        int r0;
        char *t2 = *(char **)(*(char **)data_ov007_0210342c + 0xc);
        int r4v = 0x7fff;
        r0 = 0x1000;
        int s = *(short *)t2;
        if (s == 0xb) {
            int n = *(int *)(t2 + 0xc);
            r4v = 0x3def;
            if (n <= 0)
                r0 = 0;
            else if (n < 0x1e)
                r0 = _ZN4cstd3divEii(n << 0xc, 0x1e);
        } else if (s == 0xa) {
            int n = *(int *)(t2 + 0xc);
            if (n <= 0)
                r0 = 0;
            else if (n < 0x1e)
                r0 = _ZN4cstd3divEii(n << 0xc, 0x1e);
        }
        *(u16 *)(*(char **)(*(char **)data_ov007_0210342c + 0x38) + 0x22) =
            func_ov007_020c3be0(r0, *(u16 *)(*(char **)(*(char **)data_ov007_0210342c + 0x38) + 0x22), r4v);
    }
}
