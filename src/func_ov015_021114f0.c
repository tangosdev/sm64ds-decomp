// NONMATCHING: different op / idiom (div=21). Logic verified correct vs ROM; not
// byte-matchable from C at mwccarm 1.2/sp2p3 (see notes/matching-style.md).
// Counts as decompiled, not matched.
extern void Matrix4x3_FromRotationY(void *m, short ang);
extern int _ZN5Actor18DropShadowScaleXYZER11ShadowModelR9Matrix4x35Fix12IiES5_S5_j(void *self, void *sm, void *mtx, int a, int b, int d, unsigned int e);
extern short SINE_TABLE[];
int func_ov015_021114f0(char *c) {
    int r4 = 0;
    int idx = *(unsigned short*)(c+0x8c) >> 4;
    int s = *(short*)((char*)SINE_TABLE + (idx << 2));
    int r2 = (int)(((long long)s * 0x7d0000 + 0x800) >> 12);
    int r5 = 0x8000;
    if (r2 <= 0) { r5 = (unsigned short)(r5 + 0x8000); r2 = -r2; }
    r4 = r4 + r2;
    {
        int ang = (short)(*(short*)(c+0x8e) + r5);
        Matrix4x3_FromRotationY(c+0x348, ang);
    }
    {
        int idx2 = (int)(unsigned short)((short)(*(short*)(c+0x8e) + r5)) >> 4;
        int s2 = SINE_TABLE[(idx2 << 1) + 1];
        int prod = (int)(((long long)s2 * (r4 >> 1) + 0x800) >> 12);
        *(int*)(c+0x36c) = (*(int*)(c+0x378) + prod) >> 3;
    }
    *(int*)(c+0x370) = *(int*)(c+0x37c) >> 3;
    {
        int idx3 = (int)(unsigned short)((short)(*(short*)(c+0x8e) + r5)) >> 4;
        int s3 = SINE_TABLE[idx3 << 1];
        int prod = (int)(((long long)s3 * (r4 >> 1) + 0x800) >> 12);
        *(int*)(c+0x374) = (*(int*)(c+0x380) + prod) >> 3;
    }
    if (*(unsigned char*)(c+0x397) >= 2) {
        *(int*)(c+0x370) = *(int*)(c+0x384) >> 3;
    }
    return _ZN5Actor18DropShadowScaleXYZER11ShadowModelR9Matrix4x35Fix12IiES5_S5_j(c, c+0x320, c+0x348, 0xc8000, 0x12c000, r4, 0xf);
}
