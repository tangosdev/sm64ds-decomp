extern void Matrix4x3_FromRotationY(void *m, short ang);
extern int _ZN5Actor18DropShadowScaleXYZER11ShadowModelR9Matrix4x35Fix12IiES5_S5_j(void *self, void *sm, void *mtx, int a, int b, int d, unsigned int e);
extern short data_02082214[];
int func_ov035_02111798(char *c) {
    int d1 = *(int*)(c+0x60) - *(int*)(c+0x324);
    int ad = d1 < 0 ? -d1 : d1;
    if (ad > 0x7d0000) return ad;
    int r2 = ad + 0x350000;
    int half = ad / 2;
    if (r2 < 0x350000) r2 = 0x350000;
    *(int*)(c+0xb4) = -half;
    *(int*)(c+0xb8) = (int)(r2 + ((unsigned)r2 >> 31)) >> 4;
    Matrix4x3_FromRotationY(c+0x350, *(short*)(c+0x8e));
    *(int*)(c+0x374) = *(int*)(c+0x5c) >> 3;
    *(int*)(c+0x378) = *(int*)(c+0x324) >> 3;
    *(int*)(c+0x37c) = *(int*)(c+0x64) >> 3;
    int idx = *(unsigned short*)(c+0x8c) >> 4;
    int s = data_02082214[(idx << 1) + 1];
    int sa = s < 0 ? -s : s;
    int scaled = (int)(((long long)sa * 0x26c000 + 0x800) >> 12);
    return _ZN5Actor18DropShadowScaleXYZER11ShadowModelR9Matrix4x35Fix12IiES5_S5_j(c, c+0x328, c+0x350, 0x12c000, 0x32000, scaled + 0x50000, 0xf);
}
