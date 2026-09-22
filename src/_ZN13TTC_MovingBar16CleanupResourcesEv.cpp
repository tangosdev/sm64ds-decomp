//cpp
// @symbol _ZN13TTC_MovingBar16CleanupResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "TTC_MovingBar.h"
#include "SharedFilePtr.h"
#include "dBgW.h"

int TTC_MovingBar::CleanupResources()
{
  if(((dBgW *)((char *)&(*(u8 *)&mMeshCollider)))->IsEnabled())
    ((dBgW *)((char *)&(*(u8 *)&mMeshCollider)))->Disable();
  ((SharedFilePtr *)(*(void**)(data_ov065_0211d35c + mVariant*0xc)))->Release();
  ((SharedFilePtr *)(*(void**)(data_ov065_0211d360 + mVariant*0xc)))->Release();
  return 1;
}
