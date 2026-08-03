//cpp
#include "SharedFilePtr.h"
// @symbol func_ov009_02111bd4
// recovered name: daObjMcWater_c_CleanupResources
/* recovered: renamed to Class_Method */
/* daObjMcWater_c::CleanupResources - recovered from vtable slot identity */
extern "C" {
int _ZN16MeshColliderBase9IsEnabledEv(void*);
int _ZN16MeshColliderBase7DisableEv(void*);
extern int data_ov009_02113c68[];
extern int data_ov009_02113c70[];
int func_ov009_02111bd4(char* c){
  ((SharedFilePtr *)((void*)data_ov009_02113c68))->Release();
  ((SharedFilePtr *)((void*)data_ov009_02113c70))->Release();
  if(_ZN16MeshColliderBase9IsEnabledEv(c+0x124))
    _ZN16MeshColliderBase7DisableEv(c+0x124);
  return 1;
}
}
