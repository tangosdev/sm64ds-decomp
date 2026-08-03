//cpp
// @symbol _ZN25RotatingUpDownPlatformUtm16CleanupResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "RotatingUpDownPlatformUtm.h"
#include "SharedFilePtr.h"
struct SFP { void *a, *b, *c; };
extern struct SFP data_ov091_02134c30[];
extern struct SFP data_ov091_02134c34[];

int RotatingUpDownPlatformUtm::CleanupResources()
{
  if(mSpawnParam == 0xffff) return 1;
  if(_ZN16MeshColliderBase9IsEnabledEv((char *)&mMeshCollider))
    _ZN16MeshColliderBase7DisableEv((char *)&mMeshCollider);
  ((SharedFilePtr *)(data_ov091_02134c30[mVariant].a))->Release();
  ((SharedFilePtr *)(data_ov091_02134c34[mVariant].a))->Release();
  return 1;
}
