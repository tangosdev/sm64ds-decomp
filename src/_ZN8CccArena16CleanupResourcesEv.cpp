//cpp
// @symbol _ZN8CccArena16CleanupResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "CccArena.h"
#include "SharedFilePtr.h"
#include "MeshColliderBase.h"

int CccArena::CleanupResources()
{
  if(((MeshColliderBase *)((char *)&mMeshCollider))->IsEnabled())
    ((MeshColliderBase *)((char *)&mMeshCollider))->Disable();
  ((SharedFilePtr *)(*(void**)(data_ov073_021231bc + (unsigned char)((char *)this)[0x32c]*0xc)))->Release();
  ((SharedFilePtr *)(*(void**)(data_ov073_021231c0 + (unsigned char)((char *)this)[0x32c]*0xc)))->Release();
  return 1;
}
