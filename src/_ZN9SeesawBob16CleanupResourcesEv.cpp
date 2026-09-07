//cpp
// @symbol _ZN9SeesawBob16CleanupResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "SeesawBob.h"
#include "SharedFilePtr.h"
#include "dBgW.h"

int SeesawBob::CleanupResources()
{
  if(((dBgW *)((char *)&(*(u8 *)&mMeshCollider)))->IsEnabled())
    ((dBgW *)((char *)&(*(u8 *)&mMeshCollider)))->Disable();
  ((SharedFilePtr *)(*(void**)(data_ov095_021374a0 + mVariant*0xc)))->Release();
  ((SharedFilePtr *)(*(void**)(data_ov095_021374a4 + mVariant*0xc)))->Release();
  return 1;
}
