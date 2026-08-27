// @symbol func_ov002_020bafc0
/* recovered: shared common types */
#include "common.h"
typedef int Fix12i;
typedef short s16;


struct Mtx43 { Fix12i a[12]; };

extern void Vec3_Asr(struct Vector3* d, struct Vector3* s, int sh);
extern void Matrix4x3_FromTranslation(struct Mtx43* m, Fix12i x, Fix12i y, Fix12i z);
extern void Matrix4x3_ApplyInPlaceToTranslation(struct Mtx43* m, Fix12i x, Fix12i y, Fix12i z);
extern void Matrix4x3_ApplyInPlaceToRotationZXYExt(struct Mtx43* m, int x, int y, int z);

extern struct Mtx43 data_020a0e68;

void func_ov002_020bafc0(char* self){
    struct Vector3 v;
    Vec3_Asr(&v, (struct Vector3*)(self + 0x5c), 3);
    Matrix4x3_FromTranslation(&data_020a0e68, v.x, v.y, v.z);
    Matrix4x3_ApplyInPlaceToTranslation(&data_020a0e68, 0, 0x8c00, 0);
    Matrix4x3_ApplyInPlaceToRotationZXYExt(&data_020a0e68,
        *(s16*)(self + 0x8c), *(s16*)(self + 0x8e), *(s16*)(self + 0x90));
    Matrix4x3_ApplyInPlaceToTranslation(&data_020a0e68, 0, -0x8c00, 0);
    *(struct Mtx43*)(self + 0xf0) = data_020a0e68;
}
