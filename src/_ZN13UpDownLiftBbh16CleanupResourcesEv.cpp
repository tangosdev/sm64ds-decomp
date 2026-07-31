//cpp
// @symbol _ZN13UpDownLiftBbh16CleanupResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "UpDownLiftBbh.h"
extern "C" {
extern void _ZN13SharedFilePtr7ReleaseEv(void*);
extern void* data_ov095_02136f68[];
extern void* data_ov095_02136f74[];
}

int UpDownLiftBbh::CleanupResources()
{
  if(_ZN16MeshColliderBase9IsEnabledEv((char*)&mMeshCollider))
    _ZN16MeshColliderBase7DisableEv((char*)&mMeshCollider);
  _ZN13SharedFilePtr7ReleaseEv(data_ov095_02136f68[*(int*)((char*)&mVariant)]);
  _ZN13SharedFilePtr7ReleaseEv(data_ov095_02136f74[*(int*)((char*)&mVariant)]);
  return 1;
}
