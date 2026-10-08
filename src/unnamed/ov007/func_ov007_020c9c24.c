#pragma opt_common_subs off
typedef short s16;
typedef long long s64;

extern void func_ov007_020c7a84(char *p, int f1, int f2);
extern void func_ov007_020c80c8(char *o, int a, int s);
extern void func_ov007_020c7560(void *self, int r6, int r5);
extern void func_ov007_020c7504(int *self, int b, int c);

void func_ov007_020c9c24(int *self, int arg1)
{
    int i;
    int bnd;
    int r2v, r1v;
    int off;
    int scale;
    short *sh;
    int *ctx;

    for (i = 0; i < 2; i++) {
        ctx = (int *)self[1];
        func_ov007_020c7a84((char *)((int *)((char *)self + 0x60))[i], ctx[6], ctx[7]);
        func_ov007_020c80c8((char *)((int *)((char *)self + 0x50))[i], 0, arg1);
        ctx = (int *)self[1];
        func_ov007_020c7a84((char *)((int *)((char *)self + 0x68))[i], ctx[6], ctx[7]);
        func_ov007_020c80c8((char *)((int *)((char *)self + 0x58))[i], 0, arg1);
    }

    off = self[2] * 6;
    sh = *(short **)((char *)*(int **)self + 0x2c);
    ctx = (int *)self[1];
    bnd = *(s16 *)((char *)ctx + 0x2c);
    r2v = *(s16 *)((char *)sh + off);
    r1v = *(s16 *)((char *)sh + off + 2);

    if (r2v > bnd)
        r2v = bnd;
    else if (r2v < -bnd)
        r2v = (s16)(-bnd);

    bnd = *(s16 *)((char *)ctx + 0x2e);
    if (r1v > bnd)
        r1v = bnd;
    else if (r1v < -bnd)
        r1v = (s16)(-bnd);

    scale = *(int *)((char *)ctx + 0x10);
    {
        int *d = *(int **)*(int **)*(int **)*(int **)((char *)self + 0x1c);
        d[0] = (int)(((s64)r2v * scale + 0x800) >> 0xc);
    }
    {
        int *d;
        scale = *(int *)((char *)(int *)self[1] + 0x10);
        d = *(int **)*(int **)*(int **)*(int **)((char *)self + 0x1c);
        d[1] = (int)(((s64)r1v * scale + 0x800) >> 0xc);
    }

    ctx = (int *)self[1];
    func_ov007_020c7560(*(void **)((char *)self + 0x1c), ctx[2], ctx[3]);
    func_ov007_020c7504(*(int **)((char *)self + 0x1c), 0, arg1);
}
