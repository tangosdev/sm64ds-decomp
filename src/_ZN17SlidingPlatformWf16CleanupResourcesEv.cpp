//cpp
// @symbol _ZN17SlidingPlatformWf16CleanupResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "SlidingPlatformWf.h"
extern void _ZN13SharedFilePtr7ReleaseEv(void *);

int SlidingPlatformWf::CleanupResources()
{
  if(_ZN16MeshColliderBase9IsEnabledEv((char *)&mMovingMeshCollider))
    _ZN16MeshColliderBase7DisableEv((char *)&mMovingMeshCollider);
  _ZN13SharedFilePtr7ReleaseEv(*(void**)(data_ov091_02135024 + (unsigned char)((char *)this)[0x322]*0xc));
  _ZN13SharedFilePtr7ReleaseEv(*(void**)(data_ov091_02135028 + (unsigned char)((char *)this)[0x322]*0xc));
  return 1;
}
