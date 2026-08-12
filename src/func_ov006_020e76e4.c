// @symbol func_ov006_020e76e4
// recovered name: dScMg3DEsp_c_OnYoshiTryEat
/* recovered: renamed to Class_Method, declarations from a shared header */
#include "decl_Model.h"
#include "decl_TextureTransformer.h"
#include "decl_common.h"
/* recovered: renamed to Class_Method */
/* dScMg3DEsp_c::OnYoshiTryEat - recovered from vtable slot identity */
extern void _ZN8Particle10SysTrackerD1Ev(void*);
extern int _ZTV19dScMgSingle3DBase_c;
extern void* data_020a0eac;
void* func_ov006_020e76e4(char* p){
  *(int*)p = (int)&data_ov006_0213c8c4;
  _ZN18TextureTransformerD1Ev(p + 0x51f4);
  func_ov006_020e80d8(p + 0x4fd8);
  _ZN5ModelD1Ev(p + 0x4f88);
  _ZN5ModelD1Ev(p + 0x4f38);
  *(int*)p = (int)&_ZTV19dScMgSingle3DBase_c;
  _ZN8Particle10SysTrackerD1Ev(p + 0x471c);
  _ZN11dScMgBase_cD2Ev(p);
  _ZN6Memory10DeallocateEPvP4Heap(p, data_020a0eac);
  return p;
}
