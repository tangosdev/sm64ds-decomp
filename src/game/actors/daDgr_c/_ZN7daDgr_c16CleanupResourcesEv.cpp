//cpp
// @symbol _ZN7daDgr_c16CleanupResourcesEv
/* daDgr_c::CleanupResources -- vtable slot 3. Real C++ method over the shared
   header. */
#include "daDgr_c.h"
#include "SharedFilePtr.h"
extern "C" {
extern int data_ov025_02113a68[];
extern int data_ov025_02113a60[];
}
s32 daDgr_c::CleanupResources() {
  ((SharedFilePtr *)(data_ov025_02113a68))->Release();
  ((SharedFilePtr *)(data_ov025_02113a60))->Release();
  if (mMeshCollider.IsEnabled())
    mMeshCollider.Disable();
  return 1;
}
