//cpp
// @symbol _ZN6ShipUp16CleanupResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "ShipUp.h"
#include "SharedFilePtr.h"
extern "C" {
}

int ShipUp::CleanupResources()
{
  if(_ZN16MeshColliderBase9IsEnabledEv((char*)&mMeshCollider))
    _ZN16MeshColliderBase7DisableEv((char*)&mMeshCollider);
  ((SharedFilePtr *)(data_ov016_021136e4[mModelIndex]))->Release();
  ((SharedFilePtr *)(data_ov016_021136dc[mModelIndex]))->Release();
  return 1;
}
