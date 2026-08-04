//cpp
// @symbol func_ov006_020c1f4c
/* recovered: shared common types */
#include "common.h"
typedef short s16;

extern "C" {
void Matrix4x3_FromTranslation(Matrix4x3* m, int x, int y, int z);
void Matrix4x3_ApplyInPlaceToRotationY(Matrix4x3* m, s16 a);
void Matrix4x3_ApplyInPlaceToRotationX(Matrix4x3* m, s16 a);
void Matrix4x3_ApplyInPlaceToRotationZ(Matrix4x3* m, s16 a);
extern Matrix4x3 data_020a0e68;
}

struct Vtbl {
  virtual void v0();
  virtual void v1();
  virtual void v2();
  virtual void v3();   // offset 0xc
};

struct Sub { char pad[0x120]; Matrix4x3 m; };

extern "C" void func_ov006_020c1f4c(char* c){
  Matrix4x3_FromTranslation(&data_020a0e68, *(int*)(c+0x8c), *(int*)(c+0x90), *(int*)(c+0x94));
  Matrix4x3_ApplyInPlaceToRotationY(&data_020a0e68, *(s16*)(c+0x9a));
  *(Matrix4x3*)(c+0x2c) = data_020a0e68;
  ((Vtbl*)(c+0x10))->v3();
  data_020a0e68 = (*(Sub**)(c+0x24))->m;
  Matrix4x3_ApplyInPlaceToRotationX(&data_020a0e68, *(s16*)(c+0xa4));
  Matrix4x3_ApplyInPlaceToRotationZ(&data_020a0e68, *(s16*)(c+0xa2));
  (*(Sub**)(c+0x24))->m = data_020a0e68;
}
