//cpp
struct Matrix4x3;
struct ShadowModel;

extern "C" void Matrix4x3_FromRotationY(void *m, int angle);
extern "C" void _ZN5Actor19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(
    void *thiz, struct ShadowModel *sm, struct Matrix4x3 *mtx, int a, int b, unsigned int n);

extern "C" void func_ov072_021208d8(void *thiz)
{
    unsigned char *c = (unsigned char *)thiz;

    Matrix4x3_FromRotationY(c + 0xf0, *(short *)(c + 0x8e));
    *(int *)(c + 0x114) = *(int *)(c + 0x5c) >> 3;
    *(int *)(c + 0x118) = (*(int *)(c + 0x60) + 0x13b000) >> 3;
    *(int *)(c + 0x11c) = *(int *)(c + 0x64) >> 3;

    Matrix4x3_FromRotationY(c + 0x140, *(short *)(c + 0x8e));
    *(int *)(c + 0x164) = *(int *)(c + 0x5c) >> 3;
    *(int *)(c + 0x168) = *(int *)(c + 0x60) >> 3;
    *(int *)(c + 0x16c) = *(int *)(c + 0x64) >> 3;

    _ZN5Actor19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(
        c, (struct ShadowModel *)(c + 0x188), (struct Matrix4x3 *)(c + 0x140), 0xe6000, 0x12c000, 0xf);
}
