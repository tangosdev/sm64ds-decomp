// @symbol func_ov036_02111618
/* recovered: shared common types */
#include "common.h"
extern void Vec3_Asr(void *dst, void *src, int n);
extern void Matrix4x3_FromTranslation(void *m, int x, int y, int z);
extern void Matrix4x3_ApplyInPlaceToRotationY(void *m, short ang);
extern void Matrix4x3_ApplyInPlaceToRotationX(void *m, short ang);
extern void Matrix4x3_ApplyInPlaceToRotationZ(void *m, short ang);

extern struct Matrix4x3 data_020a0e68;
void func_ov036_02111618(char *c) {
    int v[3];
    Vec3_Asr(v, c+0x5c, 3);
    Matrix4x3_FromTranslation(&data_020a0e68, v[0], v[1], v[2]);
    Matrix4x3_ApplyInPlaceToRotationY(&data_020a0e68, *(short*)(c+0x8e));
    Matrix4x3_ApplyInPlaceToRotationX(&data_020a0e68, *(short*)(c+0x8c));
    Matrix4x3_ApplyInPlaceToRotationZ(&data_020a0e68, *(short*)(c+0x90));
    *(struct Matrix4x3*)(c+0xe0) = data_020a0e68;
}
