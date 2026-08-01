//cpp
// @symbol func_ov002_020b6074
/* recovered: shared common types */
#include "common.h"
extern "C" {


extern void Matrix4x3_FromQuaternion(void* q, struct Matrix4x3* m);
extern void Vec3_Asr(struct Vector3* d, struct Vector3* s, int sh);
extern void Matrix4x3_FromTranslation(void* m, int x, int y, int z);
extern void MulMat4x3Mat4x3(struct Matrix4x3* a, void* b, void* c);
extern void Matrix4x3_ApplyInPlaceToRotationY(void* m, short a);
extern struct Matrix4x3 data_020a0e68;
void func_ov002_020b6074(char* c){
  struct Matrix4x3 q;
  struct Vector3 v;
  Matrix4x3_FromQuaternion(c+0x320, &q);
  Vec3_Asr(&v, (struct Vector3*)(c+0x5c), 3);
  Matrix4x3_FromTranslation(&data_020a0e68, v.x, v.y, v.z);
  MulMat4x3Mat4x3(&q, &data_020a0e68, &data_020a0e68);
  Matrix4x3_ApplyInPlaceToRotationY(&data_020a0e68, *(short*)(c+0x8e));
  *(struct Matrix4x3*)(c+0xf0) = data_020a0e68;

}
}
