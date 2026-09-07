//cpp
// @symbol _ZN13FortressTower16CleanupResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "FortressTower.h"
#include "SharedFilePtr.h"
#include "dBgW.h"
extern char data_ov102_0214e188[];
extern char data_ov102_0214e18c[];

int FortressTower::CleanupResources()
{
  if(((dBgW *)((char *)&(*(u8 *)&mMeshCollider)))->IsEnabled())
    ((dBgW *)((char *)&(*(u8 *)&mMeshCollider)))->Disable();
  ((SharedFilePtr *)(*(void**)(data_ov102_0214e188 + (unsigned char)((char *)this)[0x31e]*0xc)))->Release();
  ((SharedFilePtr *)(*(void**)(data_ov102_0214e18c + (unsigned char)((char *)this)[0x31e]*0xc)))->Release();
  return 1;
}
