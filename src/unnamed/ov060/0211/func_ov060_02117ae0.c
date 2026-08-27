// @symbol func_ov060_02117ae0
/* recovered: shared common types */
#include "common.h"
extern void Vec3_Asr(void* d, void* s, int sh);
extern void Matrix4x3_FromTranslation(void* m, int x, int y, int z);
extern void Matrix4x3_ApplyInPlaceToRotationX(void* mF, short angX);
extern void Matrix4x3_ApplyInPlaceToRotationZ(void* mF, short angZ);
extern struct Matrix4x3 data_020a0e68;
void func_ov060_02117ae0(char* c) {
  int v[4];
  Vec3_Asr(&v, c+0x5c, 3);
  Matrix4x3_FromTranslation(&data_020a0e68, v[0], v[1], v[2]);
  Matrix4x3_ApplyInPlaceToRotationX(&data_020a0e68, *(short*)(c+0x8c));
  Matrix4x3_ApplyInPlaceToRotationZ(&data_020a0e68, *(short*)(c+0x90));
  *(struct Matrix4x3*)(c+0x340) = data_020a0e68;
}
