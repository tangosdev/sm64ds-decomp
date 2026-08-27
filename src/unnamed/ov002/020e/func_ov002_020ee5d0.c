// @symbol func_ov002_020ee5d0
/* recovered: shared common types */
#include "common.h"
typedef short s16;


extern struct Matrix4x3 data_020a0e68;
extern void Vec3_Asr(struct Vector3 *d, struct Vector3 *s, int sh);
extern void Matrix4x3_FromTranslation(struct Matrix4x3 *m, int x, int y, int z);
extern void Matrix4x3_ApplyInPlaceToTranslation(struct Matrix4x3 *m, int x, int y, int z);
extern void Matrix4x3_ApplyInPlaceToRotationZXYExt(void *m, int x, int y, int z);

void func_ov002_020ee5d0(unsigned char *self, int arg)
{
    struct Vector3 v;
    Vec3_Asr(&v, (struct Vector3 *)(self + 0x5c), 3);
    Matrix4x3_FromTranslation(&data_020a0e68, v.x, v.y, v.z);
    Matrix4x3_ApplyInPlaceToTranslation(&data_020a0e68, 0, arg >> 3, 0);
    Matrix4x3_ApplyInPlaceToRotationZXYExt(&data_020a0e68,
        *(s16 *)(self + 0x8c), *(s16 *)(self + 0x8e), *(s16 *)(self + 0x90));
    Matrix4x3_ApplyInPlaceToTranslation(&data_020a0e68, 0, -arg >> 3, 0);
    *(struct Matrix4x3 *)(self + 0xf0) = data_020a0e68;
}
