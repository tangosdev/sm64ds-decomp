//cpp
// @symbol func_ov077_02123c6c
/* recovered: shared common types, declarations from a shared header */
#include "decl_dBgCh_Actr.h"
/* recovered: shared common types */
#include "common.h"
#include "dBgCh_Gnd.h"
extern "C" {

extern int dBgCh_Actr_UpdateDiscreteNoLava_veneer(void* w);
extern int _ZNK10dBgCh_Actr8IsOnWallEv(void* w);
extern int _ZNK11SurfaceInfo12CopyNormalToER7Vector3(void* s, struct Vector3* v);
void func_ov077_02123c6c(char* c, void* w){
  struct Vector3 nrm;
  struct Vector3 pos;
  dBgCh_Actr_UpdateDiscreteNoLava_veneer(w);
  if (_ZNK10dBgCh_Actr8IsOnWallEv(w) != 0) {
    _ZNK11SurfaceInfo12CopyNormalToER7Vector3((char*)_ZNK10dBgCh_Actr13GetWallResultEv(w) + 4, &nrm);
    *(unsigned char*)(((int)c + 0x41d)) ^= 1;
  }
  pos.x = *(int*)(c+0x5c);
  pos.y = *(int*)(c+0x60);
  pos.z = *(int*)(c+0x64);
  pos.y += 0x64000;
  dBgCh_Gnd rg;
  rg.SetObjAndPos(pos, 0);
  rg.StartDetectingWater();
  if (rg.DetectClsn() != 0) {
    int yy = rg.clsnY + 0x3c000;
    if (*(int*)(c+0x60) < yy) *(int*)(c+0x60) = yy;
  }
  rg.StopDetectingWater();
}
}
