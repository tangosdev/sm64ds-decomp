extern void Matrix4x3_FromRotationY(void *m, short ang);
extern int _ZN5Actor18DropShadowScaleXYZER11ShadowModelR9Matrix4x35Fix12IiES5_S5_j(void *self, void *sm, void *mtx, int a, int b, int d, unsigned int e);
extern short SINE_TABLE[];
int func_ov023_0211124c(char *c) {
    int idx = *(unsigned short*)(c+0x8c) >> 4;
    int s = *(short*)((char*)SINE_TABLE + (idx << 2));
    int r5 = (int)(((long long)s * 0x49c000 + 0x800) >> 12);
    Matrix4x3_FromRotationY(c+0x34c, *(short*)(c+0x8e));
    *(int*)(c+0x370) = *(int*)(c+0x5c) >> 3;
    *(int*)(c+0x374) = *(int*)(c+0x60) >> 3;
    *(int*)(c+0x378) = *(int*)(c+0x64) >> 3;
    return _ZN5Actor18DropShadowScaleXYZER11ShadowModelR9Matrix4x35Fix12IiES5_S5_j(c, c+0x324, c+0x34c, 0x258000, 0x32000, -r5, 0xf);
}
