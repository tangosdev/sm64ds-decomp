//cpp
#include "SharedFilePtr.h"
#include "MeshColliderBase.h"
// @symbol func_ov009_02111bd4
// recovered name: daObjMcWater_c_CleanupResources
/* recovered: renamed to Class_Method */
/* daObjMcWater_c::CleanupResources - recovered from vtable slot identity */
extern "C" {
extern int data_ov009_02113c68[];
extern int data_ov009_02113c70[];
int func_ov009_02111bd4(char* c){
  ((SharedFilePtr *)((void*)data_ov009_02113c68))->Release();
  ((SharedFilePtr *)((void*)data_ov009_02113c70))->Release();
  if(((MeshColliderBase *)(c+0x124))->IsEnabled())
    ((MeshColliderBase *)(c+0x124))->Disable();
  return 1;
}
}
