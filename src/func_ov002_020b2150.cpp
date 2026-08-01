//cpp
// @symbol func_ov002_020b2150
// recovered name: VirtualDoor_Kill
/* recovered: renamed to Class_Method, declarations from a shared header */
#include "decl_common.h"
/* recovered: renamed to Class_Method */
/* daChScene_c::Kill - recovered from vtable slot identity */
extern "C" {
extern int _ZNK12WithMeshClsn8IsOnWallEv(void*);
extern int _ZNK12WithMeshClsn10IsOnGroundEv(void*);
void func_ov002_020b2150(char* c){
  func_ov002_020b13e0(c);
  if(_ZNK12WithMeshClsn8IsOnWallEv(c+0x1ac)) *(int*)(c+0x98)=0;
  if(_ZNK12WithMeshClsn10IsOnGroundEv(c+0x1ac)){
    *(int*)(c+0xa8)=0;
    *(int*)(c+0x9c)=0;
    *(int*)(c+0x3a4)=4;
  }
}
}
