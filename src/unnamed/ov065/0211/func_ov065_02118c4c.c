// @symbol func_ov065_02118c4c
/* recovered: shared common types */
#include "common.h"
extern void Vec3_Asr(void* d, void* s, int sh);
extern void Matrix4x3_FromTranslation(void* m, int x, int y, int z);
extern void Matrix4x3_ApplyInPlaceToRotationY(void* mF, short angY);
extern void Matrix4x3_ApplyInPlaceToRotationX(void* mF, short angX);
extern struct Matrix4x3 data_020a0e68;
void func_ov065_02118c4c(char* c) {
  int v[4];
  Vec3_Asr(&v, c+0x5c, 3);
  Matrix4x3_FromTranslation(&data_020a0e68, v[0], v[1], v[2]);
  Matrix4x3_ApplyInPlaceToRotationY(&data_020a0e68, *(short*)(c+0x8e));
  Matrix4x3_ApplyInPlaceToRotationX(&data_020a0e68, *(short*)(c+0x8c));
  *(struct Matrix4x3*)(c+0x10c) = data_020a0e68;
}
