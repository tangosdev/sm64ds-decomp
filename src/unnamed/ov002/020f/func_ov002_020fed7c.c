// @symbol func_ov002_020fed7c
/* recovered: shared common types */
#include "common.h"
extern void Vec3_Asr(void *dst, void *src, int n);
extern void Matrix4x3_FromTranslation(void *m, int x, int y, int z);
extern void Matrix4x3_ApplyInPlaceToRotationXYZExt(void *m, short rx, short ry, short rz);

extern struct Matrix4x3 data_020a0e68;
void func_ov002_020fed7c(char *c) {
    int v[3];
    Vec3_Asr(v, c+0x5c, 3);
    Matrix4x3_FromTranslation(&data_020a0e68, v[0], v[1], v[2]);
    Matrix4x3_ApplyInPlaceToRotationXYZExt(&data_020a0e68,
        *(short*)(c+0x8c), *(short*)(c+0x8e), *(short*)(c+0x90));
    *(struct Matrix4x3*)(c+0x31c) = data_020a0e68;
}
