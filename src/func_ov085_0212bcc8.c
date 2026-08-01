// @symbol func_ov085_0212bcc8
/* recovered: shared common types */
#include "common.h"
extern void Vec3_Asr(struct Vector3* d, struct Vector3* s, int sh);
extern void Matrix4x3_FromTranslation(void* m, int x, int y, int z);
extern void Matrix4x3_ApplyInPlaceToRotationXYZExt(void* m, int x, int y, int z);
extern void MulMat4x3Mat4x3(void* a, void* b, void* dst);
extern void _ZN5Actor19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(
    void* thiz, void* sm, void* mtx, int f, int a, unsigned int b);

extern struct Matrix4x3 data_020a0e68;

void func_ov085_0212bcc8(char* c)
{
    char tmp[0x30];
    struct Vector3 t;
    Vec3_Asr(&t, (struct Vector3*)(c + 0x5c), 3);
    Matrix4x3_FromTranslation(&data_020a0e68, t.x, t.y, t.z);
    Matrix4x3_ApplyInPlaceToRotationXYZExt(&data_020a0e68,
        *(short*)(c + 0x8c), *(short*)(c + 0x8e), *(short*)(c + 0x90));
    *(struct Matrix4x3*)(c + 0x31c) = data_020a0e68;
    MulMat4x3Mat4x3(*(void**)(c + 0x314), (void*)(c + 0x31c), tmp);
    Matrix4x3_FromTranslation(&data_020a0e68,
        *(int*)(c + 0x5c) >> 3,
        (*(int*)(c + 0x60) - 0xe000) >> 3,
        *(int*)(c + 0x64) >> 3);
    *(struct Matrix4x3*)(c + 0x390) = data_020a0e68;
    _ZN5Actor19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(
        c, (void*)(c + 0x368), (void*)(c + 0x390), 0x46000, 0x258000, 0xf);
}
