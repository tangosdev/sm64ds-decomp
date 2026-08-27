//cpp
// @symbol func_ov077_021251d0
/* recovered: shared common types */
#include "common.h"
extern "C" {
void Vec3_Asr(void* d, void* s, int sh);
void Matrix4x3_FromTranslation(void* m, int x, int y, int z);
void Matrix4x3_ApplyInPlaceToTranslation(void* m, int x, int y, int z);
void Matrix4x3_ApplyInPlaceToRotationZXYExt(void* m, int x, int y, int z);

extern Matrix4x3 data_020a0e68;
}
struct Base {
  virtual void v0();
  virtual void v1();
  virtual void v2();
  virtual void v3();
  virtual void v4();
  virtual void v5();
  virtual void v6();
  virtual void v7();
  virtual void v8();
  virtual void v9();
  virtual void v10();
  virtual void v11();
  virtual void v12();
  virtual void v13();
  virtual void v14();
  virtual void v15();
  virtual void v16();
  virtual void v17();
  virtual void v18();
  virtual void v19();
  virtual void v20();
  virtual void v21();
  virtual void v22();
  virtual void v23();
  virtual void v24();
  virtual void v25();
  virtual void v26();
  virtual void v27();
  virtual void v28();
  virtual int m();   // slot 0x74/4 = 29
};
extern "C" void func_ov077_021251d0(void* c)
{
  char* r4 = (char*)c;
  int v[3];
  Vec3_Asr(v, r4+0x5c, 3);
  Matrix4x3_FromTranslation(&data_020a0e68, v[0], v[1], v[2]);
  Base* b = (Base*)c;
  int r = b->m();
  Matrix4x3_ApplyInPlaceToTranslation(&data_020a0e68, 0, r >> 3, 0);
  Matrix4x3_ApplyInPlaceToRotationZXYExt(&data_020a0e68,
      *(short*)(r4+0x8c), *(short*)(r4+0x8e), *(short*)(r4+0x90));
  int r2 = b->m();
  Matrix4x3_ApplyInPlaceToTranslation(&data_020a0e68, 0, (-r2) >> 3, 0);
  *(Matrix4x3*)(r4+0x140) = data_020a0e68;
}
