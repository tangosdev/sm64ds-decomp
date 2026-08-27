//cpp
// @symbol func_ov077_02123c6c
/* recovered: shared common types, declarations from a shared header */
#include "decl_dBgCh_Actr.h"
/* recovered: shared common types */
#include "common.h"
extern "C" {

struct RG { char buf[0x54]; };
extern int dBgCh_Actr_UpdateDiscreteNoLava_veneer(void* w);
extern int _ZNK10dBgCh_Actr8IsOnWallEv(void* w);
extern int _ZNK11SurfaceInfo12CopyNormalToER7Vector3(void* s, struct Vector3* v);
extern int _ZN9dBgCh_GndC1Ev(struct RG* r);
extern int _ZN9dBgCh_Gnd12SetObjAndPosERK7Vector3P8dActor_c(struct RG* r, struct Vector3* v, void* a);
extern int _ZN5dBgCh19StartDetectingWaterEv(struct RG* r);
extern int _ZN9dBgCh_Gnd10DetectClsnEv(struct RG* r);
extern int _ZN5dBgCh18StopDetectingWaterEv(struct RG* r);
extern int _ZN9dBgCh_GndD1Ev(struct RG* r);
void func_ov077_02123c6c(char* c, void* w){
  struct Vector3 nrm;
  struct Vector3 pos;
  struct RG rg;
  dBgCh_Actr_UpdateDiscreteNoLava_veneer(w);
  if (_ZNK10dBgCh_Actr8IsOnWallEv(w) != 0) {
    _ZNK11SurfaceInfo12CopyNormalToER7Vector3((char*)_ZNK10dBgCh_Actr13GetWallResultEv(w) + 4, &nrm);
    *(unsigned char*)(((int)c + 0x41d)) ^= 1;
  }
  pos.x = *(int*)(c+0x5c);
  pos.y = *(int*)(c+0x60);
  pos.z = *(int*)(c+0x64);
  pos.y += 0x64000;
  _ZN9dBgCh_GndC1Ev(&rg);
  _ZN9dBgCh_Gnd12SetObjAndPosERK7Vector3P8dActor_c(&rg, &pos, 0);
  _ZN5dBgCh19StartDetectingWaterEv(&rg);
  if (_ZN9dBgCh_Gnd10DetectClsnEv(&rg) != 0) {
    int yy = *(int*)((char*)&rg + 0x44) + 0x3c000;
    if (*(int*)(c+0x60) < yy) *(int*)(c+0x60) = yy;
  }
  _ZN5dBgCh18StopDetectingWaterEv(&rg);
  _ZN9dBgCh_GndD1Ev(&rg);
}
}
