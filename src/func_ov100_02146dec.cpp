//cpp
// @symbol func_ov100_02146dec
// recovered name: daObjPathLift_c_OnYoshiTryEat
/* recovered: renamed to Class_Method, declarations from a shared header */
#include "decl_Actor.h"
#include "decl_Model.h"
#include "decl_MovingMeshCollider.h"
#include "decl_ShadowModel.h"
#include "decl_common.h"
/* recovered: renamed to Class_Method */
/* daObjPathLift_c::OnYoshiTryEat - recovered from vtable slot identity */
extern "C" {
extern void __destroy_arr(void* arr, int count, int size, void(*dtor)(void*));
extern void* data_ov002_0210af70[];
extern void* _ZTV17ExclamationSwitch[];
extern void* data_020a0eac;


void* func_ov100_02146dec(char* p){
  *(void***)p = (void**)data_ov100_0214857c;
  _ZN11ShadowModelD1Ev(p+0x450);
  *(void***)p = (void**)data_ov002_0210af70;
  __destroy_arr(p+0x320, 3, 0x50, _ZN5ModelD1Ev);
  *(void***)p = (void**)_ZTV17ExclamationSwitch;
  _ZN18MovingMeshColliderD1Ev(p+0x124);
  _ZN5ModelD1Ev(p+0xd4);
  _ZN5ActorD2Ev(p);
  _ZN6Memory10DeallocateEPvP4Heap(p, data_020a0eac);
  return p;
}
}
