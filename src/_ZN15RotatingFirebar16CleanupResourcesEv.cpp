//cpp
// @symbol _ZN15RotatingFirebar16CleanupResourcesEv
/* recovered: named members + shared header, real C++ method */
#include "RotatingFirebar.h"
#include "SharedFilePtr.h"
#include "MeshColliderBase.h"
extern "C" {
extern int data_ov064_0211adbc[];
}

int RotatingFirebar::CleanupResources()
{
  if (((MeshColliderBase *)((char*)&mMeshCollider))->IsEnabled())
    ((MeshColliderBase *)((char*)&mMeshCollider))->Disable();
  ((SharedFilePtr *)((void*)data_ov064_0211adbc[0]))->Release();
  ((SharedFilePtr *)((void*)data_ov064_0211adbc[1]))->Release();
  return 1;
}
