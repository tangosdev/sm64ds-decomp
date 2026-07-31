//cpp
// @symbol _ZN22RotatingUpDownPlatform16CleanupResourcesEv
/* recovered: named members + shared header, real C++ method */
#include "RotatingUpDownPlatform.h"
extern "C" {
extern int _ZN13SharedFilePtr7ReleaseEv(void*);
extern int _ZN16MeshColliderBase7DisableEv(void*);
extern void* data_ov091_021344fc[];
extern void* data_ov091_021344f4[];
}

int RotatingUpDownPlatform::CleanupResources()
{
  _ZN13SharedFilePtr7ReleaseEv(data_ov091_021344fc[mVariant]);
  _ZN13SharedFilePtr7ReleaseEv(data_ov091_021344f4[mVariant]);
  _ZN16MeshColliderBase7DisableEv((char*)&mMeshCollider);
  return 1;
}
