//cpp
// @symbol _ZN12FortressWall16CleanupResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "FortressWall.h"
#include "SharedFilePtr.h"
#include "dBgW.h"
extern char data_ov079_02128058[];
extern char data_ov079_0212805c[];

int FortressWall::CleanupResources()
{
  if(((dBgW *)((char *)&(*(u8 *)&mMeshCollider)))->IsEnabled())
    ((dBgW *)((char *)&(*(u8 *)&mMeshCollider)))->Disable();
  ((SharedFilePtr *)(*(void**)(data_ov079_02128058 + (unsigned char)((char *)this)[0x31e]*0xc)))->Release();
  ((SharedFilePtr *)(*(void**)(data_ov079_0212805c + (unsigned char)((char *)this)[0x31e]*0xc)))->Release();
  return 1;
}
