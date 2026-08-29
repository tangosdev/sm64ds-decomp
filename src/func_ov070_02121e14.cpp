//cpp
#include "dBgCh_Gnd.h"

extern "C" {
extern void _ZN8dActor_c19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(void* a, void* sm, void* m, int f1, int f2, unsigned int j);
void func_ov070_02121e14(char* c) {
  int f;
  *(int*)(c + 0x310) = *(int*)(c + 0x5c) >> 3;
  *(int*)(c + 0x314) = *(int*)(c + 0x60) >> 3;
  *(int*)(c + 0x318) = *(int*)(c + 0x64) >> 3;
  dBgCh_Gnd rg;
  rg.SetObjAndPos(*(Vector3*)(c + 0x5c), (dActor_c*)c);
  if (rg.DetectClsn() != 0)
    f = (*(int*)(c + 0x60) - rg.clsnY) + 0x1e000;
  else
    f = 0x12c000;
  _ZN8dActor_c19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(c, c + 0xd4, c + 0x2ec, 0x64000, f, 0xf);
}
}
