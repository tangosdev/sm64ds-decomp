//cpp
// @symbol _ZN10SlidingIce16CleanupResourcesEv
/* recovered: named members + shared header, real C++ method */
#include "SlidingIce.h"
#include "SharedFilePtr.h"
#include "MeshColliderBase.h"
extern char data_ov027_02113be8[];
extern char data_ov027_02113be0[];

int SlidingIce::CleanupResources()
{
  unsigned char ok = (mActorID==0x5d);
  if(ok){ ((MeshColliderBase *)((char *)&mMeshCollider))->Disable(); }
  ((SharedFilePtr *)(data_ov027_02113be8))->Release();
  ((SharedFilePtr *)(data_ov027_02113be0))->Release();
  return 1;
}
