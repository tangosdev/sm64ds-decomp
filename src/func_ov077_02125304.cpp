//cpp
struct dActor_c;
struct ShadowModel;
struct Matrix4x3;

extern "C" {
void func_ov077_021251d0(char *t);
void func_ov077_02125290(char *t);
void _ZN8dActor_c19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(dActor_c *thisp, ShadowModel *sm, Matrix4x3 *mtx, int rad, int t, unsigned char j);
void func_ov077_02125304(char *c) {
    int b = (int)((*(int*)(c+0xb0) & 0x40000) != 0);
    if (b != 0) return;
    if (*(int*)(c+0x3d8) == 5) func_ov077_021251d0(c);
    else func_ov077_02125290(c);
    *(int*)(c+0x3c4) = *(int*)(c+0x5c) >> 3;
    *(int*)(c+0x3c8) = *(int*)(c+0x60) >> 3;
    *(int*)(c+0x3cc) = *(int*)(c+0x64) >> 3;
    _ZN8dActor_c19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j((dActor_c*)c, (ShadowModel*)(c+0x188), (Matrix4x3*)(c+0x3a0), 0x50000, 0x320000, 0xf);
}
}
