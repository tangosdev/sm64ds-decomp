enum { false, true };

extern void Matrix4x3_FromRotationY(void *m, short ang);
extern void _ZN5Actor18DropShadowScaleXYZER11ShadowModelR9Matrix4x35Fix12IiES5_S5_j(void *self, void *sm, void *mtx, int a, int b, int d, unsigned int e);
extern short data_02082214[];
extern signed char data_0209f2f8;
extern unsigned char data_0209f2d8;

void func_ov100_02146e70(char *c) {
    int r6;
    int r5;
    int belse;
    int idx;
    int s0;
    int s1;
    int t;

    if (data_0209f2f8 == 13) return;
    if (*(int*)(c+0x4ac) > *(int*)(c+0x60)) return;

    belse = (*(int*)(c+0x60) - *(int*)(c+0x4ac)) + 0x64000;

    idx = *(unsigned short*)(c+0x8c) >> 4;
    s0 = data_02082214[idx << 1];
    r5 = (int)(((long long)s0 * 0x17c000 + 0x800) >> 12);
    if (r5 < 0) r5 = -r5;
    s1 = data_02082214[(idx << 1) + 1];
    r6 = (int)(((long long)s1 * 0x17c000 + 0x800) >> 12);
    if (r6 < 0) r6 = -r6;

    Matrix4x3_FromRotationY(c+0x478, *(short*)(c+0x8e));
    *(int*)(c+0x49c) = *(int*)(c+0x5c) >> 3;
    *(int*)(c+0x4a0) = (*(int*)(c+0x60) - r5) >> 3;
    *(int*)(c+0x4a4) = *(int*)(c+0x64) >> 3;

    t = data_0209f2d8;
    t = t == 1;
    if (t != false) {
        _ZN5Actor18DropShadowScaleXYZER11ShadowModelR9Matrix4x35Fix12IiES5_S5_j(c, c+0x450, c+0x478, 0x118000, 0x7d0000, r6, 0xf);
    } else {
        _ZN5Actor18DropShadowScaleXYZER11ShadowModelR9Matrix4x35Fix12IiES5_S5_j(c, c+0x450, c+0x478, 0x118000, belse, r6, 0xf);
    }
}
