//cpp
// @symbol func_ov063_0211c6f8
/* recovered: shared common types */
#include "common.h"

extern "C" void Matrix4x3_FromTranslation(void* m, int x, int y, int z);
extern "C" void Matrix4x3_ApplyInPlaceToRotationXYZExt(void* m, int x, int y, int z);
extern "C" void _ZN18MovingMeshCollider9TransformERK9Matrix4x3s(void* self, void* mtx, short s);
extern "C" struct Matrix4x3 data_020a0e68;
extern "C" void func_ov063_0211c6f8(char* c);
extern "C" void func_ov063_0211c6f8(char* c) {
  Matrix4x3_FromTranslation(&data_020a0e68, *(int*)(c+0x5c), *(int*)(c+0x60), *(int*)(c+0x64));
  Matrix4x3_ApplyInPlaceToRotationXYZExt(&data_020a0e68, *(short*)(c+0x8c), *(short*)(c+0x8e), *(short*)(c+0x90));
  *(struct Matrix4x3*)(c+0x324) = data_020a0e68;
  _ZN18MovingMeshCollider9TransformERK9Matrix4x3s(c+0x15c, c+0x324, *(short*)(c+0x8e));
}
