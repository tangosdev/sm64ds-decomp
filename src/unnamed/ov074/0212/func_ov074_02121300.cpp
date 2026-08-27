//cpp
// @symbol func_ov074_02121300
/* recovered: shared common types */
#include "common.h"
extern "C" {
typedef int Fix12;


extern void Vec3_Asr(struct Vector3* d, struct Vector3* s, int sh);
extern void Matrix4x3_FromTranslation(void* m, int x, int y, int z);
extern void Matrix4x3_ApplyInPlaceToRotationY(void* m, short a);
extern void Matrix4x3_ApplyInPlaceToScale(void* m, Fix12 x, Fix12 y, Fix12 z);
extern struct Matrix4x3 data_020a0e68;
void func_ov074_02121300(char* c){
  struct Vector3 v;
  Vec3_Asr(&v, (struct Vector3*)(c+0x5c), 3);
  Matrix4x3_FromTranslation(&data_020a0e68, v.x, v.y, v.z);
  Matrix4x3_ApplyInPlaceToRotationY(&data_020a0e68, *(short*)(c+0x8e));
  Matrix4x3_ApplyInPlaceToScale(&data_020a0e68, *(int*)(c+0x80), *(int*)(c+0x84), *(int*)(c+0x88));
  *(struct Matrix4x3*)(c+0x37c) = data_020a0e68;
}
}
