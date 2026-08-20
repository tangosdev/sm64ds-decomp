// @symbol func_ov060_02117a64
/* recovered: shared common types */
#include "common.h"
extern void Matrix4x3_FromTranslation(void *m, int x, int y, int z);
extern void Matrix4x3_ApplyInPlaceToRotationX(void *m, short ang);
extern void Matrix4x3_ApplyInPlaceToRotationZ(void *m, short ang);
extern void _ZN10dBgW_KcMbg9TransformERK9Matrix4x3s(void *self, void *m, short s);

extern struct Matrix4x3 data_020a0e68;
void func_ov060_02117a64(char *c) {
    Matrix4x3_FromTranslation(&data_020a0e68, *(int*)(c+0x5c), *(int*)(c+0x60), *(int*)(c+0x64));
    Matrix4x3_ApplyInPlaceToRotationX(&data_020a0e68, *(short*)(c+0x8c));
    Matrix4x3_ApplyInPlaceToRotationZ(&data_020a0e68, *(short*)(c+0x90));
    *(struct Matrix4x3*)(c+0x2ec) = data_020a0e68;
    _ZN10dBgW_KcMbg9TransformERK9Matrix4x3s(c+0x374, c+0x2ec, *(short*)(c+0x8e));
}
