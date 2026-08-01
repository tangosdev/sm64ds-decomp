//cpp
extern "C" {
void Matrix4x3_FromTranslation(void *m, int x, int y, int z);
void _ZN5Actor19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(void *thisp, void *sm, void *mtx, int rad, int t, unsigned int j);
void func_ov063_021169c4(char *c) {
    Matrix4x3_FromTranslation(c+0x31c, *(int*)(c+0x5c)>>3, *(int*)(c+0x60)>>3, *(int*)(c+0x64)>>3);
    _ZN5Actor19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(c, c+0x350, c+0x31c, 0x64000, 0x64000, 0xf);
}
}
