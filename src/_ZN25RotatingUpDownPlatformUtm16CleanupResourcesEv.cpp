//cpp
// @symbol _ZN25RotatingUpDownPlatformUtm16CleanupResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "RotatingUpDownPlatformUtm.h"
extern void _ZN13SharedFilePtr7ReleaseEv(void *);
struct SFP { void *a, *b, *c; };
extern struct SFP data_ov091_02134c30[];
extern struct SFP data_ov091_02134c34[];

int RotatingUpDownPlatformUtm::CleanupResources()
{
  if(mSpawnParam == 0xffff) return 1;
  if(_ZN16MeshColliderBase9IsEnabledEv((char *)&mMeshCollider))
    _ZN16MeshColliderBase7DisableEv((char *)&mMeshCollider);
  _ZN13SharedFilePtr7ReleaseEv(data_ov091_02134c30[mVariant].a);
  _ZN13SharedFilePtr7ReleaseEv(data_ov091_02134c34[mVariant].a);
  return 1;
}
