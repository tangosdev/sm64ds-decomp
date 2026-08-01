//cpp
// @symbol func_ov027_02111994
/* recovered: shared common types */
#include "common.h"

extern "C" void Matrix4x3_FromRotationY(void*, short);
extern "C" void _ZN18MovingMeshCollider9TransformERK9Matrix4x3s(void*, void*, short);
extern "C" void func_ov027_02111994(char *c) {
  Matrix4x3_FromRotationY(c+0x33c, *(short*)(c+0x8e));
  *(int*)(c+0x360) = *(int*)(c+0x5c) >> 3;
  *(int*)(c+0x364) = *(int*)(c+0x60) >> 3;
  *(int*)(c+0x368) = *(int*)(c+0x64) >> 3;
  *(struct Matrix4x3*)(c+0x2ec) = *(struct Matrix4x3*)(c+0x33c);
  *(int*)(c+0x310) = *(int*)(c+0x5c);
  *(int*)(c+0x314) = *(int*)(c+0x60);
  *(int*)(c+0x318) = *(int*)(c+0x64);
  _ZN18MovingMeshCollider9TransformERK9Matrix4x3s(c+0x124, c+0x2ec, *(short*)(c+0x8e));
}
