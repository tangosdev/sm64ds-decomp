//cpp
// @symbol func_ov025_02111384
// recovered name: daDgr_c_CleanupResources
/* recovered: renamed to Class_Method, declarations from a shared header */
#include "decl_common.h"
#include "SharedFilePtr.h"
#include "MeshColliderBase.h"
/* recovered: renamed to Class_Method */
/* daDgr_c::CleanupResources - recovered from vtable slot identity */
extern "C" {
extern int data_ov025_02113a68[];
int func_ov025_02111384(char* c) {
  ((SharedFilePtr *)(data_ov025_02113a68))->Release();
  ((SharedFilePtr *)(data_ov036_02113a60))->Release();
  if (((MeshColliderBase *)(c+0x124))->IsEnabled())
    ((MeshColliderBase *)(c+0x124))->Disable();
  return 1;
}
}
