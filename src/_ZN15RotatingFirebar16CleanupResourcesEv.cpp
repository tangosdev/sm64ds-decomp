//cpp
// @symbol _ZN15RotatingFirebar16CleanupResourcesEv
/* recovered: named members + shared header, real C++ method */
#include "RotatingFirebar.h"
extern "C" {
int _ZN16MeshColliderBase9IsEnabledEv(void* self);
void _ZN16MeshColliderBase7DisableEv(void* self);
void _ZN13SharedFilePtr7ReleaseEv(void* self);
extern int data_ov064_0211adbc[];
}

int RotatingFirebar::CleanupResources()
{
  if (_ZN16MeshColliderBase9IsEnabledEv((char*)&mMeshCollider))
    _ZN16MeshColliderBase7DisableEv((char*)&mMeshCollider);
  _ZN13SharedFilePtr7ReleaseEv((void*)data_ov064_0211adbc[0]);
  _ZN13SharedFilePtr7ReleaseEv((void*)data_ov064_0211adbc[1]);
  return 1;
}
