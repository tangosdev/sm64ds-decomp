//cpp
// @symbol func_ov064_02119f1c
/* recovered: shared common types */
#include "common.h"
extern "C" {


extern void Vec3_Asr(struct Vector3* d, struct Vector3* s, int sh);
extern void Matrix4x3_FromTranslation(void* m, int x, int y, int z);
extern void Matrix4x3_ApplyInPlaceToRotationXYZExt(void* m, int x, int y, int z);
extern void _ZN9ModelBase12ApplyOpacityEj(void* m, unsigned int j, int k);
extern struct Matrix4x3 data_020a0e68;
void func_ov064_02119f1c(char* c){
  struct Vector3 v;
  Vec3_Asr(&v, (struct Vector3*)(c+0x5c), 3);
  Matrix4x3_FromTranslation(&data_020a0e68, v.x, v.y, v.z);
  Matrix4x3_ApplyInPlaceToRotationXYZExt(&data_020a0e68, *(short*)(c+0x8c), *(short*)(c+0x8e), *(short*)(c+0x90));
  _ZN9ModelBase12ApplyOpacityEj(c+0x30c, *(unsigned char*)(c+0x380), 1);
  *(struct Matrix4x3*)(c+0x328) = data_020a0e68;
}
}
