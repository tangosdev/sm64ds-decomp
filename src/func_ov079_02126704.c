// @symbol func_ov079_02126704
/* recovered: shared common types, declarations from a shared header */
#include "decl_common.h"
/* recovered: shared common types */
#include "common.h"
typedef int Fix12;
typedef short s16;
extern void _ZN8dActor_c19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(void* self, void* sm, void* mtx, Fix12 a, Fix12 b, unsigned int g);



void func_ov079_02126704(char* c) {
  Matrix4x3_FromRotationXYZExt(c+0x328, *(s16*)(c+0x8c), *(s16*)(c+0x8e), *(s16*)(c+0x90));
  *(int*)(c+0x34c) = *(int*)(c+0x5c) >> 3;
  *(int*)(c+0x350) = *(int*)(c+0x60) >> 3;
  *(int*)(c+0x354) = *(int*)(c+0x64) >> 3;
  *(struct Matrix4x3*)(c+0x378) = *(struct Matrix4x3*)(c+0x328);
  _ZN8dActor_c19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(c, c+0x3ac, c+0x328, 0x50000, 0x50000, 0xf);
}
