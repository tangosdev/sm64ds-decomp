#include "types.h"
// @symbol func_ov065_0211b40c
/* recovered: shared common types */
#include "common.h"
void _ZN8dActor_c19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(
   void* self, void* sm, struct Matrix4x3* m, int f, int g, u32 h);
void func_ov065_0211b40c(char* c){
  *(struct Matrix4x3*)(c+0x34c) = *(struct Matrix4x3*)(c+0xf0);
  *(int*)(c+0x374) = (*(int*)(c+0x320) + 0x32000) >> 3;
  _ZN8dActor_c19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(
     c, c+0x324, (struct Matrix4x3*)(c+0x34c), 0x258000, 0xc8000, 0xf);
}
