//cpp
// @symbol _ZN17SlidingPlatformWf16CleanupResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "SlidingPlatformWf.h"
#include "SharedFilePtr.h"
#include "dBgW.h"

int SlidingPlatformWf::CleanupResources()
{
  if(((dBgW *)((char *)&(*(u8 *)&mMeshCollider)))->IsEnabled())
    ((dBgW *)((char *)&(*(u8 *)&mMeshCollider)))->Disable();
  ((SharedFilePtr *)(*(void**)(data_ov091_02135024 + (unsigned char)((char *)this)[0x322]*0xc)))->Release();
  ((SharedFilePtr *)(*(void**)(data_ov091_02135028 + (unsigned char)((char *)this)[0x322]*0xc)))->Release();
  return 1;
}
