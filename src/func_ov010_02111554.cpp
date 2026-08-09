//cpp
// @symbol func_ov010_02111554
// recovered name: daObjC1_Trap_c_CleanupResources
/* recovered: renamed to Class_Method, declarations from a shared header */
#include "decl_common.h"
#include "SharedFilePtr.h"
#include "MeshColliderBase.h"
/* recovered: renamed to Class_Method */
/* daObjC1_Trap_c::CleanupResources - recovered from vtable slot identity */
extern "C" int func_ov010_02111554(char *self){
  if(((MeshColliderBase *)(self+0x124))->IsEnabled()){
    ((MeshColliderBase *)(self+0x124))->Disable();
  }
  if((*(unsigned int*)(self+8) & 0xff) != 0xff){
    ((SharedFilePtr *)(&data_ov010_02112d08))->Release();
    ((SharedFilePtr *)(&data_ov010_02112d00))->Release();
  }
  return 1;
}
