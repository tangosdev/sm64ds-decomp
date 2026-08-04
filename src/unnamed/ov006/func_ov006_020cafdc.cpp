//cpp
// @symbol func_ov006_020cafdc
/* recovered: shared common types */
#include "common.h"
extern "C" {

extern struct Matrix4x3 data_020a0e68;
void Matrix4x3_FromTranslation(struct Matrix4x3 *mF, int x, int y, int z);
void Matrix4x3_ApplyInPlaceToRotationY(struct Matrix4x3 *mF, short angY);
void func_ov006_020cafdc(char *o) {
  Matrix4x3_FromTranslation(&data_020a0e68, *(int*)(o+0x1c), *(int*)(o+0x20), *(int*)(o+0x24));
  Matrix4x3_ApplyInPlaceToRotationY(&data_020a0e68, *(short*)(o+0x4a));
  *(struct Matrix4x3*)(o+0x88) = data_020a0e68;
}
}
