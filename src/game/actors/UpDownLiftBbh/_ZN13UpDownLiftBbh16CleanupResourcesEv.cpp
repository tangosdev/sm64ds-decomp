//cpp
// @symbol _ZN13UpDownLiftBbh16CleanupResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "UpDownLiftBbh.h"
#include "SharedFilePtr.h"
#include "dBgW.h"
extern "C" {
extern void* data_ov095_02136f68[];
extern void* data_ov095_02136f74[];
}

int UpDownLiftBbh::CleanupResources()
{
  if(((dBgW *)((char*)&mMeshCollider))->IsEnabled())
    ((dBgW *)((char*)&mMeshCollider))->Disable();
  ((SharedFilePtr *)(data_ov095_02136f68[*(int*)((char*)&mVariant)]))->Release();
  ((SharedFilePtr *)(data_ov095_02136f74[*(int*)((char*)&mVariant)]))->Release();
  return 1;
}
