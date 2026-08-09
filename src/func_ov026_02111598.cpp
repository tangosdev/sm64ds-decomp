//cpp
// @symbol func_ov026_02111598
// recovered name: daObjWlPolelift_c_InitResources
/* recovered: renamed to Class_Method, declarations from a shared header */
#include "decl_PathPtr.h"
#include "decl_common.h"
/* recovered: renamed to Class_Method */
/* daObjWlPolelift_c::InitResources - recovered from vtable slot identity */
struct V3 { int x,y,z; V3(){} V3(const V3&o){x=o.x;y=o.y;z=o.z;} };
extern "C" {
extern void* _ZN5Model8LoadFileER13SharedFilePtr(void*);
extern void _ZN9ModelBase7SetFileEP8BMD_Fileii(void*, void*, int, int);
extern void _ZN11ShadowModel10InitCuboidEv(void*);
extern void _ZN7PathPtr6FromIDEj(void*, unsigned int);
extern void _ZN25MovingCylinderClsnWithPos4InitEP5ActorRK7Vector35Fix12IiES6_jj(void*, void*, V3*, int, int, unsigned int, unsigned int);
extern int data_0209caa0[];
extern void func_ov026_021112a4(char*);
int func_ov026_02111598(char *c){
  void *f = _ZN5Model8LoadFileER13SharedFilePtr((void*)data_ov026_02113ea0);
  _ZN9ModelBase7SetFileEP8BMD_Fileii(c+0xd4, f, 1, -1);
  if((data_0209caa0[1] & 0x204) == 0) return 0;
  _ZN11ShadowModel10InitCuboidEv(c+0x188);
  _ZN7PathPtr6FromIDEj(c+0x164, *(unsigned int*)(c+8) & 0xff);
  *(int*)(c+0x16c) = _ZNK7PathPtr8NumNodesEv(c+0x164);
  *(int*)(c+0x180) = 1;
  V3 g = *(V3*)&data_ov026_02113a9c;
  _ZN25MovingCylinderClsnWithPos4InitEP5ActorRK7Vector35Fix12IiES6_jj(c+0x124, c, &g, 0x50000, 0x12c000, 0x80000c, 0);
  func_ov026_021112a4(c);
  return 1;
}
}
