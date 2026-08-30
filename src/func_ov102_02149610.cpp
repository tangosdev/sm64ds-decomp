//cpp
#include "dBgCh_Gnd.h"

struct V3 {
  int x, y, z;
  V3(int a, int b, int d) { x = a; y = b; z = d; }
  V3() {}
};

extern "C" int func_ov102_02149610(char *c){
  V3 pos(*(int*)(c+0x5c), *(int*)(c+0x60)+0x28000, *(int*)(c+0x64));
  dBgCh_Gnd rg;
  rg.SetObjAndPos(*(Vector3*)&pos, 0);
  rg.mProbeHeight = 0x3e8000;
  int r = *(int*)(c+0x60);
  if (rg.DetectClsn()) r = rg.clsnY;
  return r;
}
