//cpp
// @symbol func_ov091_02133098
/* recovered: shared common types */
#include "common.h"
extern "C" {
void _ZN5Actor18DropShadowScaleXYZER11ShadowModelR9Matrix4x35Fix12IiES5_S5_j(
    void* a, void* sm, void* mtx, int f, int t1, int t2, unsigned int x);

void func_ov091_02133098(void* c){
  char* r = (char*)c;
  int off = 0x20000;
  int d = *(int*)(r+0x60) - *(int*)(r+0x394);
  if(d <= 0x14000){ d = 0x14000; off = 0; }
  int rad = (int)(((long long)d * 0x60 + 0x800) >> 12);
  int* pb8 = (int*)(((int)r + 0xb8));
  int v1 = *(int*)(*(char**)(r+0x320)+0x10) - rad;
  if(v1 < 0xa000) v1 = 0xa000;
  int v2 = *(int*)(*(char**)(r+0x320)+0x14) - rad;
  if(v2 < 0xa000) v2 = 0xa000;
  *(int*)(r+0xb8) = d + 0x8c000;
  {
    int t = *(int*)(r+0xb8);
    if(t < 0x200000) t = 0x200000;
    *(int*)(r+0xb8) = t;
  }
  *pb8 = *pb8 >> 3;
  *(Matrix4x3*)(r+0x360) = *(Matrix4x3*)(r+0xf0);
  *(int*)(r+0x384) = *(int*)(r+0x5c) >> 3;
  *(int*)(r+0x388) = (*(int*)(r+0x60) - off) >> 3;
  *(int*)(r+0x38c) = *(int*)(r+0x64) >> 3;
  _ZN5Actor18DropShadowScaleXYZER11ShadowModelR9Matrix4x35Fix12IiES5_S5_j(
      r, r+0x338, r+0x360, v1, d + 0x28000, v2, 0xf);
}
}
