//cpp
// @symbol _ZN8CccArena16CleanupResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "CccArena.h"
#include "SharedFilePtr.h"
#include "dBgW.h"

int CccArena::CleanupResources()
{
  if(((dBgW *)((char *)&mMeshCollider))->IsEnabled())
    ((dBgW *)((char *)&mMeshCollider))->Disable();
  ((SharedFilePtr *)(*(void**)(data_ov073_021231bc + mVariant*0xc)))->Release();
  ((SharedFilePtr *)(*(void**)(data_ov073_021231c0 + mVariant*0xc)))->Release();
  return 1;
}
