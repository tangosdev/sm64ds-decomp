//cpp
// @symbol func_ov010_02111554
// recovered name: daObjC1_Trap_c_CleanupResources
/* recovered: renamed to Class_Method, declarations from a shared header */
#include "decl_common.h"
#include "SharedFilePtr.h"
/* recovered: renamed to Class_Method */
/* daObjC1_Trap_c::CleanupResources - recovered from vtable slot identity */
extern "C" char data_ov025_02112d08[];
extern "C" int func_ov010_02111554(char *self){
  if(_ZN16MeshColliderBase9IsEnabledEv(self+0x124)){
    _ZN16MeshColliderBase7DisableEv(self+0x124);
  }
  if((*(unsigned int*)(self+8) & 0xff) != 0xff){
    ((SharedFilePtr *)(data_ov025_02112d08))->Release();
    ((SharedFilePtr *)(data_ov024_02112d00))->Release();
  }
  return 1;
}
