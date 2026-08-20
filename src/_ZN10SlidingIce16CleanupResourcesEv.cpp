//cpp
// @symbol _ZN10SlidingIce16CleanupResourcesEv
/* recovered: named members + shared header, real C++ method */
#include "SlidingIce.h"
#include "SharedFilePtr.h"
#include "dBgW.h"
extern char data_ov027_02113be8[];
extern char data_ov027_02113be0[];

int SlidingIce::CleanupResources()
{
  unsigned char ok = (actorID==0x5d);
  if(ok){ ((dBgW *)((char *)&(*(u8 *)&mMeshCollider)))->Disable(); }
  ((SharedFilePtr *)(data_ov027_02113be8))->Release();
  ((SharedFilePtr *)(data_ov027_02113be0))->Release();
  return 1;
}
