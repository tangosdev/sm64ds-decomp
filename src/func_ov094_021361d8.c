// @symbol func_ov094_021361d8
/* recovered: shared common types */
#include "common.h"
extern void Vec3_Asr(struct Vector3* d, struct Vector3* s, int sh);
extern void Matrix4x3_FromTranslation(struct Matrix4x3* m, Fix12i x, Fix12i y, Fix12i z);
extern void Matrix4x3_ApplyInPlaceToRotationXYZExt(void* m, int x, int y, int z);
extern void _ZN9ModelBase12ApplyOpacityEjj(void* self, unsigned int opacity, unsigned int unused);
extern void MulMat4x3Mat4x3(const int* a, const int* b, int* dst);
extern void _ZN8dActor_c19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(
    void* self, void* sm, struct Matrix4x3* m, Fix12i fx, int t, unsigned int u);

extern struct Matrix4x3 data_020a0e68;

void func_ov094_021361d8(char* self){
    struct Matrix4x3 out;
    struct Vector3 v;
    Vec3_Asr(&v, (struct Vector3*)(self + 0x5c), 3);
    Matrix4x3_FromTranslation(&data_020a0e68, v.x, v.y, v.z);
    Matrix4x3_ApplyInPlaceToRotationXYZExt(&data_020a0e68,
        *(s16*)(self + 0x8c), *(s16*)(self + 0x8e), *(s16*)(self + 0x90));
    _ZN9ModelBase12ApplyOpacityEjj(self + 0x30c, *(unsigned char*)(self + 0x3e4), 1);
    *(struct Matrix4x3*)(self + 0x328) = data_020a0e68;
    MulMat4x3Mat4x3((const int*)(*(char**)(self + 0x320) + 0x30),
        (const int*)(self + 0x328), out.m);
    Matrix4x3_FromTranslation(&data_020a0e68,
        *(Fix12i*)(self + 0x5c) >> 3,
        (*(Fix12i*)(self + 0x60) - 0x38000) >> 3,
        *(Fix12i*)(self + 0x64) >> 3);
    *(struct Matrix4x3*)(self + 0x398) = data_020a0e68;
    _ZN8dActor_c19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(
        self, self + 0x370, (struct Matrix4x3*)(self + 0x398), 0x64000, 0x320000, 0xf);
}
