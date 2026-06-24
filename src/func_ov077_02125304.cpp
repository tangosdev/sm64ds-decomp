//cpp
extern "C" {
void func_ov077_021251d0(char *t);
void func_ov077_02125290(char *t);
void _ZN5Actor19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(void *thisp, void *sm, void *mtx, int rad, int t, unsigned int j);
void func_ov077_02125304(char *c) {
    int b = (int)((*(int*)(c+0xb0) & 0x40000) != 0);
    if (b != 0) return;
    if (*(int*)(c+0x3d8) == 5) func_ov077_021251d0(c);
    else func_ov077_02125290(c);
    *(int*)(c+0x3c4) = *(int*)(c+0x5c) >> 3;
    *(int*)(c+0x3c8) = *(int*)(c+0x60) >> 3;
    *(int*)(c+0x3cc) = *(int*)(c+0x64) >> 3;
    _ZN5Actor19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(c, c+0x188, c+0x3a0, 0x50000, 0x320000, 0xf);
}
}
