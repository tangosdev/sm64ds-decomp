//cpp
// @symbol _ZN15RotatingFirebar16CleanupResourcesEv
/* recovered: named members + shared header, real C++ method */
#include "RotatingFirebar.h"
#include "SharedFilePtr.h"
extern "C" {
int _ZN16MeshColliderBase9IsEnabledEv(void* self);
void _ZN16MeshColliderBase7DisableEv(void* self);
extern int data_ov064_0211adbc[];
}

int RotatingFirebar::CleanupResources()
{
  if (_ZN16MeshColliderBase9IsEnabledEv((char*)&mMeshCollider))
    _ZN16MeshColliderBase7DisableEv((char*)&mMeshCollider);
  ((SharedFilePtr *)((void*)data_ov064_0211adbc[0]))->Release();
  ((SharedFilePtr *)((void*)data_ov064_0211adbc[1]))->Release();
  return 1;
}
