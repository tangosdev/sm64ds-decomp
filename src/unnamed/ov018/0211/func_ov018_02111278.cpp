//cpp
// @symbol func_ov018_02111278
/* recovered: shared common types */
#include "common.h"
extern "C" {


void Vec3_Asr(struct Vector3* d, struct Vector3* s, int n);
void Matrix4x3_FromTranslation(struct Matrix4x3* mF, int x, int y, int z);
void Matrix4x3_ApplyInPlaceToRotationY(struct Matrix4x3* mF, short angY);
void Matrix4x3_ApplyInPlaceToRotationX(struct Matrix4x3* mF, short angX);
extern struct Matrix4x3 data_020a0e68;

void func_ov018_02111278(char* c) {
  struct Vector3 t;
  Vec3_Asr(&t, (struct Vector3*)(c+0x5c), 3);
  Matrix4x3_FromTranslation(&data_020a0e68, t.x, t.y, t.z);
  Matrix4x3_ApplyInPlaceToRotationY(&data_020a0e68, *(short*)(c+0x8e));
  Matrix4x3_ApplyInPlaceToRotationX(&data_020a0e68, *(short*)(c+0x8c));
  Matrix4x3_ApplyInPlaceToRotationY(&data_020a0e68, 0x4000);
  *(struct Matrix4x3*)(c+0xf0) = data_020a0e68;
}
}
