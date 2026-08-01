//cpp
// @symbol func_ov006_020c76e0
/* recovered: shared common types */
#include "common.h"
extern "C" {
extern void Matrix4x3_FromTranslation(void *m, int x, int y, int z);
extern void Matrix4x3_ApplyInPlaceToRotationY(void *m, short angY);

extern struct Matrix4x3 data_020a0e68;
void func_ov006_020c76e0(char *c) {
  Matrix4x3_FromTranslation(&data_020a0e68, *(int*)(c+0x14), *(int*)(c+0x18), *(int*)(c+0x1c));
  Matrix4x3_ApplyInPlaceToRotationY(&data_020a0e68, *(short*)(c+0x2e));
  *(struct Matrix4x3*)(c+0x68) = data_020a0e68;
}
}
