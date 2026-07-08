// NONMATCHING: different op / idiom (div=4). Logic verified correct vs ROM; not
// byte-matchable from C at mwccarm 1.2/sp2p3 (see notes/matching-style.md).
// Counts as decompiled, not matched.
extern unsigned short KS_FRAME_COUNTER;
extern int INV_VIEW_MATRIX_ASR_3;
extern void func_0201f138(void);
extern void MulVec3Mat4x3(void *out, void *m, void *v);
extern void Vec3_LslInPlace(void *v, int sh);
extern int _Z14ApproachLinearRiii(int *p, int target, int step);
extern void func_0201ef38(void);
extern void _ZN3G2x13SetBlendAlphaEPVttttt(volatile void *reg,
    unsigned short a, unsigned short b, int c, int d);

int func_ov002_020f7410(unsigned char *self, unsigned char *in, int sel)
{
    int a = in[0] << 12;
    int b = in[1] << 8;
    int t;
    if (a != 0 && sel == KS_FRAME_COUNTER) {
        register int *p = (int *)(*(unsigned char **)(self + 0xdc) + 0x50);
        p[0] = -0x5d00;
        p[1] = 0x1800;
        p[2] = -0x16000;
        *(short *)(self + 0x8c) = 0;
        *(short *)(self + 0x8e) = 0xffffb3c0;
        *(short *)(self + 0x90) = 0;
        func_0201f138();
    }
    MulVec3Mat4x3(*(unsigned char **)(self + 0xdc) + 0x50, &INV_VIEW_MATRIX_ASR_3, self + 0x5c);
    Vec3_LslInPlace(self + 0x5c, 3);
    if (_Z14ApproachLinearRiii((int *)(self + 0xf4), a, b) != 0 && a == 0 && b != 0)
        func_0201ef38();
    *(char *)(self + 0x102) = *(int *)(self + 0xf4) >> 12;
    {
        unsigned char r = *(unsigned char *)(self + 0x102);
        if (r != 0 || a != 0) {
            t = (r * 0xa00) >> 12;
            if (t > 0x10) t = 0x10;
            _ZN3G2x13SetBlendAlphaEPVttttt((volatile void *)0x4000050, 8, 1, t, 0x10 - t);
        }
    }
    return 1;
}
