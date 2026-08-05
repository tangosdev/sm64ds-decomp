//cpp
// @symbol _ZN22RotatingUpDownPlatform16CleanupResourcesEv
/* recovered: named members + shared header, real C++ method */
#include "RotatingUpDownPlatform.h"
#include "SharedFilePtr.h"
#include "MeshColliderBase.h"
extern "C" {
extern void* data_ov091_021344fc[];
extern void* data_ov091_021344f4[];
}

int RotatingUpDownPlatform::CleanupResources()
{
  ((SharedFilePtr *)(data_ov091_021344fc[mVariant]))->Release();
  ((SharedFilePtr *)(data_ov091_021344f4[mVariant]))->Release();
  ((MeshColliderBase *)((char*)&mMeshCollider))->Disable();
  return 1;
}
