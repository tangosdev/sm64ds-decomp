//cpp
// @symbol _ZN17SlidingPlatformWf16CleanupResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "SlidingPlatformWf.h"
#include "SharedFilePtr.h"
#include "MeshColliderBase.h"

int SlidingPlatformWf::CleanupResources()
{
  if(((MeshColliderBase *)((char *)&mMovingMeshCollider))->IsEnabled())
    ((MeshColliderBase *)((char *)&mMovingMeshCollider))->Disable();
  ((SharedFilePtr *)(*(void**)(data_ov091_02135024 + (unsigned char)((char *)this)[0x322]*0xc)))->Release();
  ((SharedFilePtr *)(*(void**)(data_ov091_02135028 + (unsigned char)((char *)this)[0x322]*0xc)))->Release();
  return 1;
}
