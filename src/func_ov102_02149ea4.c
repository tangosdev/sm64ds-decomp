extern int func_ov102_02149610(char *c);
extern void Matrix4x3_FromRotationY(void *m, int angle);
extern void _ZN5Actor18DropShadowScaleXYZER11ShadowModelR9Matrix4x35Fix12IiES5_S5_j(
    void *self, void *shadow, void *mtx, int fix, int t1, int t2, unsigned int n);
extern unsigned char data_0209f2d8;

void func_ov102_02149ea4(char *c)
{
    int r4, r5, r2v;
    int b0, b1;

    b0 = (*(int*)(c + 0xb0) & 8) ? 1 : 0;
    if (b0 != 0) {
        b1 = (*(volatile unsigned char *)&data_0209f2d8 == 1) ? 1 : 0;
        if (b1 == 0) goto skipcall;
    }
    *(int*)(c + 0x3e4) = func_ov102_02149610(c);
skipcall:
    r5 = *(int*)(c + 0x60) - *(int*)(c + 0x3e4);
    if (r5 <= 0x1000) r5 = 0x1000;
    r4 = (int)(((long long)r5 * 0x180 + 0x800) >> 12);
    r4 = 0xb4000 - r4;
    r2v = r5 + 0x214000;
    if (r4 < 0xa000) r4 = 0xa000;
    if (r2v < 0x200000) r2v = 0x200000;
    *(int*)(c + 0xb4) = -((int)((r5 + 0x14000) + ((unsigned)(r5 + 0x14000) >> 31)) >> 1);
    *(int*)(c + 0xb8) = (int)(r2v + ((unsigned)r2v >> 31)) >> 4;
    r4 = (int)(((long long)r4 * *(int*)(c + 0x80) + 0x800) >> 12);
    Matrix4x3_FromRotationY(c + 0x3ac, *(short*)(c + 0x8e));
    *(int*)(c + 0x3d0) = *(int*)(c + 0x5c) >> 3;
    *(int*)(c + 0x3d4) = (*(int*)(c + 0x60) - 0x20000) >> 3;
    *(int*)(c + 0x3d8) = *(int*)(c + 0x64) >> 3;
    _ZN5Actor18DropShadowScaleXYZER11ShadowModelR9Matrix4x35Fix12IiES5_S5_j(
        c, c + 0x384, c + 0x3ac, r4, r5 + 0x14000, r4, 0xf);
}
