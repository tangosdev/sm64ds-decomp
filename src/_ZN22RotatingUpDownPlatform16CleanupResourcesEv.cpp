//cpp
// @symbol _ZN22RotatingUpDownPlatform16CleanupResourcesEv
/* recovered: named members + shared header, real C++ method */
#include "RotatingUpDownPlatform.h"
#include "SharedFilePtr.h"
extern "C" {
extern int _ZN16MeshColliderBase7DisableEv(void*);
extern void* data_ov091_021344fc[];
extern void* data_ov091_021344f4[];
}

int RotatingUpDownPlatform::CleanupResources()
{
  ((SharedFilePtr *)(data_ov091_021344fc[mVariant]))->Release();
  ((SharedFilePtr *)(data_ov091_021344f4[mVariant]))->Release();
  _ZN16MeshColliderBase7DisableEv((char*)&mMeshCollider);
  return 1;
}
