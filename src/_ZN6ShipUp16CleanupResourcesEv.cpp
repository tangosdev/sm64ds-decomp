//cpp
// @symbol _ZN6ShipUp16CleanupResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "ShipUp.h"
extern "C" {
extern void _ZN13SharedFilePtr7ReleaseEv(void*);
}

int ShipUp::CleanupResources()
{
  if(_ZN16MeshColliderBase9IsEnabledEv((char*)&mMeshCollider))
    _ZN16MeshColliderBase7DisableEv((char*)&mMeshCollider);
  _ZN13SharedFilePtr7ReleaseEv(data_ov016_021136e4[mModelIndex]);
  _ZN13SharedFilePtr7ReleaseEv(data_ov016_021136dc[mModelIndex]);
  return 1;
}
