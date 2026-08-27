extern void Matrix4x3_FromRotationY(void *m, short ang);
extern int _ZN8dActor_c18DropShadowScaleXYZER11ShadowModelR9Matrix4x35Fix12IiES5_S5_j(void *self, void *sm, void *mtx, int a, int b, int d, unsigned int e);
int func_ov065_0211a550(char *c) {
    int d1 = *(int*)(c+0x60) - *(int*)(c+0x394);
    int ad = d1 < 0 ? -d1 : d1;
    if (ad > 0x7d0000) return ad;
    Matrix4x3_FromRotationY(c+0x35c, *(short*)(c+0x8e));
    *(int*)(c+0x380) = *(int*)(c+0x5c) >> 3;
    *(int*)(c+0x384) = (*(int*)(c+0x394) + 0x1000) >> 3;
    *(int*)(c+0x388) = *(int*)(c+0x64) >> 3;
    int b = (int)(*(unsigned short*)(c+0xc) == 0x6f);
    if (b != 0)
        return _ZN8dActor_c18DropShadowScaleXYZER11ShadowModelR9Matrix4x35Fix12IiES5_S5_j(c, c+0x334, c+0x35c, 0x1f4000, 0x32000, 0x3e8000, 0xf);
    return _ZN8dActor_c18DropShadowScaleXYZER11ShadowModelR9Matrix4x35Fix12IiES5_S5_j(c, c+0x334, c+0x35c, 0x1f4000, 0x32000, 0x320000, 0xf);
}
