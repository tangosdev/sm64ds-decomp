#include "types.h"
typedef struct G {
    int f0, f4, f8, fc, f10, f14, f18, f1c, f20, f24, f28, f2c, f30, f34;
    char f38[0x10];
    int f48, f4c;
} G;

enum Fmt {
    FMT_A = 0,
    FMT_B = 1
};

extern G data_020a5634;
extern void _ZN4CP1527FlushAndInvalidateDataCacheEjj(int a, int b);
extern int func_0204f0d8(void);
extern int func_0204f138(int a);
extern int func_0204f194(int a);
extern void func_0204f0b8(int a);
extern void func_0204f11c(int a);
extern int func_020500a4();
extern void func_0205aa10(int a, int b, int c, int d, int e, u32 f, int g, int h, int i, int j);
extern void func_0205ab6c(int a, int b, int c, u32 d, int e, int f, int g);
extern void func_0205ab28(int a, int b, int c, int d, void *e);
extern void func_0205a958(int a, int b, int c, int d);
extern void func_0205ac28(int a, int b, int c, int d);
extern void func_020524c4(void *a);
extern void func_02052494(void *a, int b, int c);

int func_020503a4(int a, int b, int c, int d, int p5, int p6, int p7, int p8,
                  int p9, int p10, int p11, int p12, int p13, int p14, int p15)
{
    int mode = 0;
    int align;
    int total;
    int blocks;
    int q;
    int fmt1;
    int fmt2;
    int m;
    int dd;
    int pp8;
    int pp10;
    u32 qlen;
    int stereo;
    int handle;
    int chunk;
    G *g;

    dd = (int)(((s64)(int)d) & ~0ULL);
    pp8 = (int)(((s64)(int)p8) & ~0ULL);
    pp10 = (int)(((s64)(int)p10) & ~0ULL);

    handle = -1;
    g = &data_020a5634;
    _ZN4CP1527FlushAndInvalidateDataCacheEjj(b, dd);
    _ZN4CP1527FlushAndInvalidateDataCacheEjj(c, dd);
    stereo = (p5 == 1) ? 1 : 0;
    align = 0xffb0ff / p9;
    if (p14 != 0) {
        q = dd;
        if (stereo == 0) q = dd >> 1;
        align = (align + 0x10) & ~0x1f;
        blocks = align >> 5;
        q = q / p13;
        total = blocks * q;
        chunk = 0x20;
        if (stereo == 0) chunk = chunk >> 1;
        chunk = chunk * blocks;
    }
    {
        fmt1 = (int)(stereo != 0 ? FMT_A : FMT_B);
        fmt2 = (int)(stereo != 0 ? FMT_B : FMT_A);
        if (a != 2) mode = 0xa;
        if (p14 != 0) {
            handle = func_0204f0d8();
            if (handle < 0) return 0;
        }
        if (func_0204f138(3) == 0) {
            if (handle >= 0) func_0204f0b8(handle);
            return 0;
        }
        if (func_0204f194(0xa) == 0) {
            if (handle >= 0) func_0204f0b8(handle);
            func_0204f11c(3);
            return 0;
        }
        qlen = (u32)dd >> 2;
        func_0205aa10(1, fmt1, b, pp8 != 0 ? 1 : 2, 0, qlen, pp10, 0, align, p11);
        func_0205ab6c(0, fmt2, b, qlen, pp8, p6, p7);
        func_0205aa10(3, fmt1, c, pp8 != 0 ? 1 : 2, 0, qlen, pp10, 0, align, p12);
        func_0205ab6c(1, fmt2, c, qlen, pp8, p6, p7);
        if (handle >= 0) {
            func_0205ab28(handle, total + chunk, total, (int)func_020500a4, g);
        }
        if (a == 1) {
            func_0205a958(3, 3, 1, 1);
        }
        if (handle >= 0) m = 1 << handle; else m = 0;
        func_0205ac28(mode, 3, m, 0);
        g->f0 = 1;
        g->f4 = a;
        g->f1c = 0xa;
        g->f20 = mode;
        g->f24 = 3;
        g->f28 = handle;
        g->f8 = p5;
        g->fc = b;
        g->f10 = c;
        g->f14 = dd;
        g->f18 = 0;
        g->f2c = p13;
        g->f30 = p14;
        g->f34 = p15;
        g->f4c = pp10;
        func_020524c4(g->f38);
        func_02052494(g->f38, pp10 << 8, 1);
        g->f48 = 0;
        return 1;
    }
}
