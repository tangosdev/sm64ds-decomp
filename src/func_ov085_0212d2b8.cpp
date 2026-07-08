//cpp
struct Vec3 { int x, y, z; };
struct Mtx43 { int w[12]; };
extern "C" Mtx43 MATRIX_SCRATCH_PAPER;
extern "C" void Vec3_Asr(Vec3* d, Vec3* s, int sh);
extern "C" void Matrix4x3_FromTranslation(Mtx43* m, int x, int y, int z);
extern "C" void Matrix4x3_ApplyInPlaceToRotationXYZExt(void *m, int x, int y, int z);
extern "C" void _ZN5Actor19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(
    void *thiz, void *sm, void *mtx, int f, int t, unsigned int u);

extern "C" void func_ov085_0212d2b8(char *thiz)
{
    Vec3 v;
    Vec3_Asr(&v, (Vec3*)(thiz + 0x5c), 3);
    Matrix4x3_FromTranslation(&MATRIX_SCRATCH_PAPER, v.x, v.y, v.z);
    Matrix4x3_ApplyInPlaceToRotationXYZExt(&MATRIX_SCRATCH_PAPER,
        *(short*)(thiz + 0x8c), *(short*)(thiz + 0x8e), *(short*)(thiz + 0x90));
    *(Mtx43*)(thiz + 0x12c) = MATRIX_SCRATCH_PAPER;
    Matrix4x3_FromTranslation(&MATRIX_SCRATCH_PAPER,
        *(int*)(thiz + 0x5c) >> 3,
        (*(int*)(thiz + 0x60) - 0x32000) >> 3,
        *(int*)(thiz + 0x64) >> 3);
    _ZN5Actor19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(
        thiz, thiz + 0x160, thiz + 0x12c, 0x23000, 0x12c000, 0xf);
}
