// NONMATCHING: different op / idiom (div=34). Logic verified correct vs ROM; not
// byte-matchable from C at mwccarm 1.2/sp2p3 (see notes/matching-style.md).
// Counts as decompiled, not matched.
int func_ov102_02149610(char *c);
void Matrix4x3_FromRotationY(void* m, int angle);
void _ZN5Actor18DropShadowScaleXYZER11ShadowModelR9Matrix4x35Fix12IiES5_S5_j(
    void* thiz, void* sm, void* mtx, int a, int b, int c2, unsigned int d);

extern unsigned char CURRENT_GAMEMODE;

void func_ov102_02149ea4(char *c) {
    {
        int b1 = (*(int*)(c + 0xb0) & 8) != 0;
        if (b1) {
            int b2 = (CURRENT_GAMEMODE == 1);
            if (b2 == 0) goto skipcall;
        }
        *(int*)(c + 0x3e4) = func_ov102_02149610(c);
    }
skipcall:
    {
        int r5 = *(int*)(c + 0x60) - *(int*)(c + 0x3e4);
        int v1, r4v, t2;
        if (r5 <= 0x1000) r5 = 0x1000;
        v1 = (int)(((long long)r5 * 0x180 + 0x800) >> 12);
        r4v = 0xb4000 - v1;
        {
            int half = (r5 + 0x14000);
            half = half + ((unsigned)half >> 31);
            half = half >> 1;
            *(int*)(c + 0xb4) = -half;
        }
        if (r4v < 0xa000) r4v = 0xa000;
        t2 = r5 + 0x214000;
        if (t2 < 0x200000) t2 = 0x200000;
        *(int*)(c + 0xb8) = (t2 + ((unsigned)t2 >> 31)) >> 4;
        r4v = (int)(((long long)r4v * *(int*)(c + 0x80) + 0x800) >> 12);
        Matrix4x3_FromRotationY(c + 0x3ac, *(short*)(c + 0x8e));
        *(int*)(c + 0x3d0) = *(int*)(c + 0x5c) >> 3;
        *(int*)(c + 0x3d4) = (*(int*)(c + 0x60) - 0x20000) >> 3;
        *(int*)(c + 0x3d8) = *(int*)(c + 0x64) >> 3;
        _ZN5Actor18DropShadowScaleXYZER11ShadowModelR9Matrix4x35Fix12IiES5_S5_j(
            c, c + 0x384, c + 0x3ac, r4v, r5 + 0x14000, r4v, 0xf);
    }

}
