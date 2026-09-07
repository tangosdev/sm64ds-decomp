//cpp
// @symbol func_ov085_0212d2b8
/* recovered: shared common types */
#include "common.h"


extern "C" Matrix4x3 data_020a0e68;
extern "C" void Vec3_Asr(Vector3* d, Vector3* s, int sh);
extern "C" void Matrix4x3_FromTranslation(Matrix4x3* m, int x, int y, int z);
extern "C" void Matrix4x3_ApplyInPlaceToRotationXYZExt(void *m, int x, int y, int z);
extern "C" void _ZN8dActor_c19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(
    void *thiz, void *sm, void *mtx, int f, int t, unsigned int u);

extern "C" void func_ov085_0212d2b8(char *thiz)
{
    Vector3 v;
    Vec3_Asr(&v, (Vector3*)(thiz + 0x5c), 3);
    Matrix4x3_FromTranslation(&data_020a0e68, v.x, v.y, v.z);
    Matrix4x3_ApplyInPlaceToRotationXYZExt(&data_020a0e68,
        *(short*)(thiz + 0x8c), *(short*)(thiz + 0x8e), *(short*)(thiz + 0x90));
    *(Matrix4x3*)(thiz + 0x12c) = data_020a0e68;
    Matrix4x3_FromTranslation(&data_020a0e68,
        *(int*)(thiz + 0x5c) >> 3,
        (*(int*)(thiz + 0x60) - 0x32000) >> 3,
        *(int*)(thiz + 0x64) >> 3);
    _ZN8dActor_c19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(
        thiz, thiz + 0x160, thiz + 0x12c, 0x23000, 0x12c000, 0xf);
}
