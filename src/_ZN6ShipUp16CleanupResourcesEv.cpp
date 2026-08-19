//cpp
// @symbol _ZN6ShipUp16CleanupResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "ShipUp.h"
#include "SharedFilePtr.h"
#include "dBgW.h"
extern "C" {
}

int ShipUp::CleanupResources()
{
  if(((dBgW *)((char*)&mMeshCollider))->IsEnabled())
    ((dBgW *)((char*)&mMeshCollider))->Disable();
  ((SharedFilePtr *)(data_ov016_021136e4[mModelIndex]))->Release();
  ((SharedFilePtr *)(data_ov016_021136dc[mModelIndex]))->Release();
  return 1;
}
