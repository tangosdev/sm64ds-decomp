//cpp
extern "C" void Matrix4x3_FromRotationXYZExt(void *m, int x, int y, int z);
extern "C" void Matrix4x3_FromRotationY(void* m, int angle);
extern "C" void _ZN8dActor_c19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(
    void *thiz, void *sm, void *mtx, int f, int t, unsigned int u);

extern "C" void func_ov070_02121310(char *thiz)
{
    if (*(int*)(thiz + 0x3a0) == 3) {
        Matrix4x3_FromRotationXYZExt(thiz + 0xf0,
            *(short*)(thiz + 0x8c),
            *(short*)(thiz + 0x8e),
            *(short*)(thiz + 0x90));
    } else {
        Matrix4x3_FromRotationY(thiz + 0xf0, *(short*)(thiz + 0x8e));
    }
    *(int*)(thiz + 0x114) = *(int*)(thiz + 0x5c) >> 3;
    *(int*)(thiz + 0x118) = *(int*)(thiz + 0x60) >> 3;
    *(int*)(thiz + 0x11c) = *(int*)(thiz + 0x64) >> 3;
    *(int*)(thiz + 0x380) = *(int*)(thiz + 0x5c) >> 3;
    *(int*)(thiz + 0x384) = *(int*)(thiz + 0x60) >> 3;
    *(int*)(thiz + 0x388) = *(int*)(thiz + 0x64) >> 3;
    _ZN8dActor_c19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(
        thiz, thiz + 0x138, thiz + 0x35c,
        *(int*)(thiz + 0x80) * 0x46,
        *(int*)(thiz + 0x3a8), 0xf);
}
