// @symbol func_ov098_0213b584
/* recovered: shared common types */
#include "common.h"
void Vec3_Asr(struct Vector3 *d, struct Vector3 *s, int sh);
void Matrix4x3_FromTranslation(struct Matrix4x3 *m, int x, int y, int z);
void Matrix4x3_ApplyInPlaceToRotationY(struct Matrix4x3 *m, short angY);
void _ZN8dActor_c19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(
    void *thiz, void *sm, struct Matrix4x3 *m, int radHeight, int a, unsigned int b);

extern struct Matrix4x3 data_020a0e68;

void func_ov098_0213b584(char *c)
{
    struct Vector3 v;
    Vec3_Asr(&v, (struct Vector3*)(c + 0x5c), 3);
    Matrix4x3_FromTranslation(&data_020a0e68, v.x, v.y, v.z);
    *(struct Matrix4x3*)(c + 0x378) = data_020a0e68;
    Matrix4x3_ApplyInPlaceToRotationY(&data_020a0e68, *(short*)(c + 0x8e));
    *(struct Matrix4x3*)(c + 0x31c) = data_020a0e68;
    _ZN8dActor_c19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(
        c, (void*)(c + 0x350), (struct Matrix4x3*)(c + 0x378),
        *(int*)(c + 0x80) * 0xa0, 0x3e8000, 6);
}
