//cpp
// @symbol func_ov077_02123c6c
/* recovered: shared common types, declarations from a shared header */
#include "decl_WithMeshClsn.h"
/* recovered: shared common types */
#include "common.h"
extern "C" {

struct RG { char buf[0x54]; };
extern int WithMeshClsn_UpdateDiscreteNoLava_veneer(void* w);
extern int _ZNK12WithMeshClsn8IsOnWallEv(void* w);
extern int _ZNK11SurfaceInfo12CopyNormalToER7Vector3(void* s, struct Vector3* v);
extern int _ZN13RaycastGroundC1Ev(struct RG* r);
extern int _ZN13RaycastGround12SetObjAndPosERK7Vector3P5Actor(struct RG* r, struct Vector3* v, void* a);
extern int _ZN4BgCh19StartDetectingWaterEv(struct RG* r);
extern int _ZN13RaycastGround10DetectClsnEv(struct RG* r);
extern int _ZN4BgCh18StopDetectingWaterEv(struct RG* r);
extern int _ZN13RaycastGroundD1Ev(struct RG* r);
void func_ov077_02123c6c(char* c, void* w){
  struct Vector3 nrm;
  struct Vector3 pos;
  struct RG rg;
  WithMeshClsn_UpdateDiscreteNoLava_veneer(w);
  if (_ZNK12WithMeshClsn8IsOnWallEv(w) != 0) {
    _ZNK11SurfaceInfo12CopyNormalToER7Vector3((char*)_ZNK12WithMeshClsn13GetWallResultEv(w) + 4, &nrm);
    *(unsigned char*)(((int)c + 0x41d)) ^= 1;
  }
  pos.x = *(int*)(c+0x5c);
  pos.y = *(int*)(c+0x60);
  pos.z = *(int*)(c+0x64);
  pos.y += 0x64000;
  _ZN13RaycastGroundC1Ev(&rg);
  _ZN13RaycastGround12SetObjAndPosERK7Vector3P5Actor(&rg, &pos, 0);
  _ZN4BgCh19StartDetectingWaterEv(&rg);
  if (_ZN13RaycastGround10DetectClsnEv(&rg) != 0) {
    int yy = *(int*)((char*)&rg + 0x44) + 0x3c000;
    if (*(int*)(c+0x60) < yy) *(int*)(c+0x60) = yy;
  }
  _ZN4BgCh18StopDetectingWaterEv(&rg);
  _ZN13RaycastGroundD1Ev(&rg);
}
}
